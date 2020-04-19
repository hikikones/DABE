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

#define RTT_SAMPLES 10

struct reno_abe {
	s32 rtts[RTT_SAMPLES];
	s32 sum;
	u32 size;
	u32 front;
	u32 back;
};

static s32 average(struct reno_abe *ca)
{
	return ca->sum / ca->size;
}

static s32 simple_moving_average(struct reno_abe *ca)
{
	s32 prev_sum = ca->sum - ca->rtts[ca->front];
	s32 prev_avg = prev_sum / (ca->size-1);
	s32 ma;

	if (ca->rtts[ca->front] > ca->rtts[ca->back])
		ma = (ca->rtts[ca->front] - ca->rtts[ca->back]) / ca->size;
	else
		ma = (ca->rtts[ca->back] - ca->rtts[ca->front]) / ca->size;

	return prev_avg + ma;
}

static s32 weighted_moving_average(struct reno_abe *ca, s32 prev_weight, s32 curr_weight)
{
    s32 sum = 0;
    s32 index = ca->back;
	s32 curr_rtt = ca->rtts[ca->front];
	s32 prev_avg;

    s32 i = 0;
    while (i < ca->size-1) {
        sum += ca->rtts[index];
        index++;
        if (index == ca->size) index = 0;
		i++;
    }

	prev_avg = sum / (ca->size-1);
    return prev_avg * prev_weight / 100 + curr_rtt * curr_weight / 100;
}

static void print_test(struct reno_abe *ca)
{
	s32 i;
	printk("RTTS: ");
	for (i = 0; i < ca->size; i++) {
		printk(KERN_CONT "%d\t", ca->rtts[i]);
	}
	printk("RTTS: ");
	for (i = 0; i < ca->size; i++) {
		if (ca->front == i)     printk(KERN_CONT "F    \t");
        else if (ca->back == i)  printk(KERN_CONT "B    \t");
        else                    printk(KERN_CONT "     \t");
	}
	printk("SUM: %d", ca->sum);
	printk("AVG: %d", average(ca));
	printk("SMA: %d", simple_moving_average(ca));
	printk("WMA: %d\n", weighted_moving_average(ca, 80, 20));
}

static void tcp_reno_abe_init(struct sock *sk)
{
	struct reno_abe *ca = inet_csk_ca(sk);
	ca->sum = 0;
	ca->size = 0;
	ca->front = 0;
	ca->back = 0;
}

static void tcp_reno_abe_acked(struct sock *sk, const struct ack_sample *sample)
{
	//const struct tcp_sock *tp = tcp_sk(sk);
	struct reno_abe *ca = inet_csk_ca(sk);

	if (sample->rtt_us <= 0)
		return;

	ca->sum += sample->rtt_us;

	if (ca->size > 0) {
		ca->front++;
	}

    if (ca->size < RTT_SAMPLES) {
		ca->rtts[ca->front] = sample->rtt_us;
        ca->size++;
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

	//print_test(ca);
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
		if (tp->snd_cwnd < tp->snd_ssthresh) {
			used_beta = abe_ss_beta;
		}
		else
			used_beta = abe_beta;
	} else {
		/* if we see packet loss */
		if (tp->snd_cwnd < tp->snd_ssthresh)
			used_beta = ss_beta;
		else
			used_beta = beta;
	}

	printk("BETA: %d", used_beta);
	int beta_avg = (used_beta * average(ca)) / ca->rtts[ca->front];
	int beta_sma = (used_beta * simple_moving_average(ca)) / ca->rtts[ca->front];
	int beta_wma = (used_beta * weighted_moving_average(ca, 20, 80)) / ca->rtts[ca->front];
	printk("BETA AVG: %d", beta_avg);
	printk("BETA SMA: %d", beta_sma);
	printk("BETA WMA: %d", beta_wma);

	//print_test(ca);

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