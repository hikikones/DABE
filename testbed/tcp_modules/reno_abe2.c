#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <net/tcp.h>


#define RENOTCP_BETA_SCALE 1024 /* Scale factor beta calculation
                                 * max_cwnd = snd_cwnd * beta
                                 */
/* beta with loss during CA */ 
static int beta __read_mostly = 512;
/* beta with loss during SS */
static int ss_beta __read_mostly = 512;
/* beta with ECN during CA */ 
static int abe_beta __read_mostly = 819;
/* beta with ECN during SS */ 
static int abe_ss_beta __read_mostly = 819;

/* Note parameters that are used for precomputing scale factors are read-only */
module_param(beta, int, 0644);
MODULE_PARM_DESC(beta, "beta for multiplicative decrease in CA without ECN");
module_param(ss_beta, int, 0644);
MODULE_PARM_DESC(ss_beta, "beta for multiplicative decrease in SS without ECN");
module_param(abe_beta, int, 0644);
MODULE_PARM_DESC(abe_beta, "beta for multiplicative decrease in CA with ECN");
module_param(abe_ss_beta, int, 0644);
MODULE_PARM_DESC(abe_ss_beta, "beta for multiplicative decrease in SS with ECN");

#define RTT_SAMPLES 16U
//#define GRADIENT_THRESHOLD 1500S

/* GOALS
	- Set cwnd_mark when queue rises (i.e. gradient > threshold)
	- Set base_rtt when either:
		- average of gradients are stable
		- right before cwnd_mark is set
	- Reset both values above after changing ssthresh (tcp_reno_ssthresh())
	- BDP
	- Optimal delay threshold on AQM (i.e. target on CoDel and PIE)
	- Set delay threshold to speciofied AQM target.

	- For every ACK, check for ECE flag. If ECE, mark cwnd.
	- Gradient of gradient?
		- Compute gradient from min and max rtts
		- Collect gradient and compare with previous
*/

struct reno_abe {
	s32 rtts[RTT_SAMPLES];
	s32 sum;
	u32 size;
	u32 front;
	u32 back;

	s32 gradient;
	s32 sma;
	s32 ema;

	u32 cwnd_mark;
	s32 base_rtt;
};

// static s32 gradient(struct reno_abe *ca)
// {
// 	s32 curr_rtt = ca->rtts[ca->front];
// 	s32 prev_rtt = ca->front == 0 ? ca->rtts[ca->size-1] : ca->rtts[ca->front-1];

// 	return curr_rtt - prev_rtt;
// }

// static u32 threshold(struct reno_abe *ca) {
// 	return max(125U, ca->base_rtt); // TODO: fancy++
// }

// static s32 simple_moving_average(struct reno_abe *ca)
// {
// 	return ca->sum / ca->size;
// }

// static s32 simple_moving_average(struct reno_abe *ca)
// {
// 	s32 prev_sum = ca->sum - ca->rtts[ca->front];
// 	s32 prev_avg = prev_sum / (ca->size-1);
// 	s32 ma;

// 	if (ca->rtts[ca->front] > ca->rtts[ca->back])
// 		ma = (ca->rtts[ca->front] - ca->rtts[ca->back]) / ca->size;
// 	else
// 		ma = (ca->rtts[ca->back] - ca->rtts[ca->front]) / ca->size;

// 	return (ca->sum / ca->size) + ma;
// 	//return prev_avg + ma;
// }

// static s32 weighted_moving_average(struct reno_abe *ca, s32 prev_weight, s32 curr_weight)
// {
//     s32 sum = 0;
//     s32 index = ca->back;
// 	s32 curr_rtt = ca->rtts[ca->front];
// 	s32 prev_avg;

//     s32 i = 0;
//     while (i < ca->size-1) {
//         sum += ca->rtts[index];
//         index++;
//         if (index == ca->size) index = 0;
// 		i++;
//     }

// 	prev_avg = sum / (ca->size-1);
//     return prev_avg * prev_weight / 100 + curr_rtt * curr_weight / 100;
// }

// static s32 exponential_moving_average(struct reno_abe *ca, s32 weight)
// {
//     return (1 - weight) * ca->
// 	//return 0; // TODO
// }

static s32 gradient(struct reno_abe *ca)
{
	s32 grad_sum = 0;
    s32 index = ca->front;
	s32 rtt;
	s32 rtt_prev;

    s32 i = 0;
    while (i < ca->size-1) {
		rtt = ca->rtts[index];
		index--;
		if (index < 0) index = ca->size-1;
		rtt_prev = ca->rtts[index];
		grad_sum += rtt - rtt_prev;
		i++;
		//printk("RTT_CURRENT: %d \t RTT_PREVIOUS: %d \t SUM: %d", rtt, rtt_prev, grad_sum);
	}

	//return grad_sum / (ca->size-1);
	return grad_sum;
}

static void print_test(struct reno_abe *ca)
{
	s32 i;
	printk("RTTs: ");
	for (i = 0; i < ca->size; i++) {
		printk(KERN_CONT "%d\t", ca->rtts[i]);
	}
	printk("RTTs: ");
	for (i = 0; i < ca->size; i++) {
		if (ca->front == i)     printk(KERN_CONT "F    \t");
        else if (ca->back == i)  printk(KERN_CONT "B    \t");
        else                    printk(KERN_CONT "     \t");
	}
	// printk("SUM: %d", ca->sum);
	// printk("GRADIENT: %d", gradient(ca));
	// printk("AVG: %d", average(ca));
	// printk("SMA: %d", simple_moving_average(ca));
	// printk("WMA: %d", exponential_moving_average(ca, 20));
	// printk("\n");
	//printk("SUM: %d", ca->sum);
	printk("GRADIENT: %d", gradient(ca));
	printk("CWND: %u", ca->cwnd_mark);
	printk("SMA: %d", ca->sma);
	printk("EMA: %d", ca->ema);
	printk("\n");
}

static void tcp_reno_abe_init(struct sock *sk)
{
	struct reno_abe *ca = inet_csk_ca(sk);
	ca->sum = 0;
	ca->size = 0;
	ca->front = 0;
	ca->back = 0;

	ca->gradient = 0;
	ca->sma = 0;
	ca->ema = 0;

	ca->cwnd_mark = 0;
	ca->base_rtt = 0;
}

static void tcp_reno_abe_acked(struct sock *sk, const struct ack_sample *sample)
{
	const struct tcp_sock *tp = tcp_sk(sk);
	struct reno_abe *ca = inet_csk_ca(sk);

	if (sample->rtt_us <= 0)
		return;

	ca->sum += sample->rtt_us;
	ca->gradient = sample->rtt_us - ca->rtts[ca->front];
	ca->ema = (ca->ema * 875) / 1000 + (sample->rtt_us * 125) / 1000;

	if (ca->size > 0) {
		ca->front++;
	}

    if (ca->size < RTT_SAMPLES) {
		ca->rtts[ca->front] = sample->rtt_us;
        ca->size++;
		ca->sma = ca->sum / ca->size;
        return;
    }

    if (ca->front == RTT_SAMPLES) {
		ca->front = 0;
	}

	ca->sum -= ca->rtts[ca->back];
    ca->rtts[ca->front] = sample->rtt_us;
    ca->back = ca->front + 1;

    if (ca->back == RTT_SAMPLES) {
		ca->back = 0;
	}

	ca->sma = ca->sum / ca->size;

	print_test(ca);

	// if (gradient(ca) >= 1500 && ca->cwnd_mark == 0) {
	// 	print_test(ca);
	// 	ca->cwnd_mark = tp->snd_cwnd;
	// 	print_test(ca);
	// }
	
	// s32 last = ca->rtts[ca->front];
	// s32 prev;
	// u32 zero;
	// u32 normal;
	// if (ca->front == 0) {
	// 	zero = ca->size - 1;
	// 	prev = ca->rtts[ca->size - 1];
	// } else {
	// 	prev = ca->rtts[ca->front - 1];
	// 	normal = ca->front - 1;
	// }
	// s32 grad = last - prev;
	// printk("LAST: %d\tPREV: %d\tZERO: %u\tNORMAL: %u", last, prev, zero, normal);
	//printk("RTT: %d", sample->rtt_us);
}

void tcp_reno_abe_cwnd_event(struct sock *sk, enum tcp_ca_event ev) {
	struct tcp_sock *tp = tcp_sk(sk);
	struct reno_abe *ca = inet_csk_ca(sk);

	switch (ev) {
	case CA_EVENT_CWND_RESTART:
		// Peak
		//printk("RESTART -> CWND: %d \t RTT %d \t", tp->snd_cwnd, ca->rtts[ca->front]);

		//ca->cwnd_mark = 0;
		break;
	case CA_EVENT_COMPLETE_CWR:
		// Bottom
		//printk("CWR -> CWND: %d \t RTT %d \t", tp->snd_cwnd, ca->rtts[ca->front]);
		
		// Reset stuff
		ca->cwnd_mark = 0;
		break;
	case CA_EVENT_ECN_NO_CE:
	case CA_EVENT_ECN_IS_CE:
		printk("ECE -> CWND: %d \t RTT %d \t", tp->snd_cwnd, ca->rtts[ca->front]);
		break;
	default:
		break;
	}
}

void tcp_reno_abe_in_ack_event(struct sock *sk, u32 flags) {
	// struct tcp_sock *tp = tcp_sk(sk);
	// struct reno_abe *ca = inet_csk_ca(sk);
	if (flags == CA_ACK_ECE) {
		printk("ECE!");
	}
}

/*
 * TCP Reno congestion control
 * This is special case used for fallback as well.
 */
/* This is Jacobson's slow start and congestion avoidance.
 * SIGCOMM '88, p. 328.
 */

void tcp_reno_cong_avoid(struct sock *sk, u32 ack, u32 acked)
{
	struct tcp_sock *tp = tcp_sk(sk);

	if (!tcp_is_cwnd_limited(sk))
		return;

	/* In "safe" area, increase. */
	if (tcp_in_slow_start(tp)) {
		acked = tcp_slow_start(tp, acked);
		if (!acked)
			return;
	}
	/* In dangerous area, increase slowly. */
	tcp_cong_avoid_ai(tp, tp->snd_cwnd, acked);
}

/* Slow start threshold is half the congestion window (min 2) */
u32 tcp_reno_ssthresh(struct sock *sk)
{
	const struct tcp_sock *tp = tcp_sk(sk);

	//return max(tp->snd_cwnd >> 1U, 2U);

	/* Below code applies Alternative Backoff with ECN (ABE) from   
	* Naeem Khademi et al., "Alternative Backoff: Achieving Low Latency 
	* and High Throughput with ECN and AQM", IFIP NETWORKING 2017 and also in 
	* https://tools.ietf.org/html/draft-ietf-tcpm-alternativebackoff-ecn
	*/

	struct reno_abe *ca = inet_csk_ca(sk);
	int used_beta;

	if (sock_net(sk)->ipv4.sysctl_tcp_ecn > 0) {
		/* if ECN enabled and we see ECE */
		//printk("ECN YES");
		if (tp->snd_cwnd < tp->snd_ssthresh) {
			used_beta = abe_ss_beta;
		}
		else
			used_beta = abe_beta;
	} else {
		/* if we see packet loss */
		//printk("ECN NO");
		if (tp->snd_cwnd < tp->snd_ssthresh)
			used_beta = ss_beta;
		else
			used_beta = beta;
	}

	// printk("BETA: %d", used_beta);
	// int beta_avg = (used_beta * average(ca)) / ca->rtts[ca->front];
	// int beta_sma = (used_beta * simple_moving_average(ca)) / ca->rtts[ca->front];
	// int beta_wma = (used_beta * weighted_moving_average(ca, 20, 80)) / ca->rtts[ca->front];
	// printk("BETA AVG: %d", beta_avg);
	// printk("BETA SMA: %d", beta_sma);
	// printk("BETA WMA: %d", beta_wma);

	//print_test(ca);

	//printk("SSTHRESH -> CWND: %d \t RTT %d \t", tp->snd_cwnd, ca->rtts[ca->front]);

	// if (ca->cwnd_mark > 0)
	// 	return max(ca->cwnd_mark, 2U);

	return max((tp->snd_cwnd * used_beta) / RENOTCP_BETA_SCALE, 2U);
}

u32 tcp_reno_undo_cwnd(struct sock *sk)
{
	const struct tcp_sock *tp = tcp_sk(sk);

	return max(tp->snd_cwnd, tp->prior_cwnd);
}

struct tcp_congestion_ops tcp_reno = {
	.flags		= TCP_CONG_NON_RESTRICTED,
	.name		= "reno_abe",
	.owner		= THIS_MODULE,
	.ssthresh	= tcp_reno_ssthresh,
	.cong_avoid	= tcp_reno_cong_avoid,
	.undo_cwnd	= tcp_reno_undo_cwnd,
	.init		= tcp_reno_abe_init,
	.pkts_acked	= tcp_reno_abe_acked,
	.cwnd_event	= tcp_reno_abe_cwnd_event,
	.in_ack_event = tcp_reno_abe_in_ack_event
};

static int __init renotcp_register(void)
{
	BUILD_BUG_ON(sizeof(struct reno_abe) > ICSK_CA_PRIV_SIZE);
	return tcp_register_congestion_control(&tcp_reno);
}

static void __exit renotcp_unregister(void)
{
	tcp_unregister_congestion_control(&tcp_reno);
}

module_init(renotcp_register);
module_exit(renotcp_unregister);

MODULE_LICENSE("GPL");