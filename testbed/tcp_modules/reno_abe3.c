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

/*
	GOALS:
		- 
*/

#define SAMPLES 8U

struct abe_minmax
{
	s32 min;
	s32 max;
};


struct reno_abe {
	struct abe_minmax rtt;
	struct abe_minmax rtt_prev;
	//struct abe_minmax gradients[SAMPLES];
	struct abe_minmax gsum;
	struct abe_minmax gtail;
	u32 size;
	u32 count;
};

static void reset(struct reno_abe *ca)
{
	ca->rtt.min = 0;
	ca->rtt.max = 0;
	ca->rtt_prev.min = 0;
	ca->rtt_prev.max = 0;
	ca->gsum.min = 0;
	ca->gsum.max = 0;
	ca->gtail.min = 0;
	ca->gtail.max = 0;
	ca->size = 0;
	ca->count = 0;
}

static void tcp_reno_abe_init(struct sock *sk)
{
	struct reno_abe *ca = inet_csk_ca(sk);

	//ca->cnt = SAMPLES;
}

static void tcp_reno_abe_acked(struct sock *sk, const struct ack_sample *sample)
{
	//const struct tcp_sock *tp = tcp_sk(sk);
	struct reno_abe *ca = inet_csk_ca(sk);

	if (sample->rtt_us <= 0)
		return;

	ca->rtt.min = min_not_zero(ca->rtt.min, sample->rtt_us);
	ca->rtt.max = max(ca->rtt.max, sample->rtt_us);
}

void tcp_reno_abe_cwnd_event(struct sock *sk, enum tcp_ca_event ev)
{
	struct tcp_sock *tp = tcp_sk(sk);
	struct reno_abe *ca = inet_csk_ca(sk);

	switch (ev) {
	case CA_EVENT_CWND_RESTART: /* Peak */
		// TODO: Reset stuff
		reset(ca);
		break;
	case CA_EVENT_COMPLETE_CWR: /* Bottom */
		reset(ca);
		break;
	case CA_EVENT_ECN_NO_CE:
	case CA_EVENT_ECN_IS_CE:
		break;
	default:
		break;
	}
}

void tcp_reno_cong_avoid(struct sock *sk, u32 ack, u32 acked)
{
	struct tcp_sock *tp = tcp_sk(sk);
	struct reno_abe *ca = inet_csk_ca(sk);

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

	
	// Calculate gradient

	printk("COUNT: %u\t", ca->count);

	s32 gmin = ca->rtt.min - ca->rtt_prev.min;
	s32 gmax = ca->rtt.max - ca->rtt_prev.max;

	if (ca->count == 0) {
		ca->gtail.min = gmin;
		ca->gtail.max = gmax;
		printk("GTAIL_MIN: %d\tGTAIL_MAX: %d\t", ca->gtail.min, ca->gtail.max);
	}
	
	if (ca->size < SAMPLES) {
		ca->size++;
	}

	ca->gsum.min += gmin - ca->gtail.min;
	ca->gsum.max += gmax - ca->gtail.max;


	ca->rtt_prev = ca->rtt;

	printk("GMIN: %d\tGMAX: %d\t", gmin, gmax);
	printk("GSUM_MIN: %d\tGSUM_MAX: %d\t", ca->gsum.min, ca->gsum.max);
	printk("RTT_MIN: %d\tRTT_MAX: %d\t", ca->rtt.min, ca->rtt.max);
	printk("\n");

	ca->count++;

	if (ca->count == SAMPLES) {
		ca->count = 0;
	}

	if (ca->size == SAMPLES) {
		ca->gsum.min -= ca->gtail.min;
		ca->gsum.max -= ca->gtail.max;
	}

	// ca->sum += sample->rtt_us;
	// ca->gradient = sample->rtt_us - ca->rtts[ca->front];
	// ca->ema = (ca->ema * 875) / 1000 + (sample->rtt_us * 125) / 1000;

	// if (ca->size > 0) {
	// 	ca->front++;
	// }

    // if (ca->size < RTT_SAMPLES) {
	// 	ca->rtts[ca->front] = sample->rtt_us;
    //     ca->size++;
	// 	ca->sma = ca->sum / ca->size;
    //     return;
    // }

    // if (ca->front == RTT_SAMPLES) {
	// 	ca->front = 0;
	// }

	// ca->sum -= ca->rtts[ca->back];
    // ca->rtts[ca->front] = sample->rtt_us;
    // ca->back = ca->front + 1;

    // if (ca->back == RTT_SAMPLES) {
	// 	ca->back = 0;
	// }

	// ca->sma = ca->sum / ca->size;
}

/* Slow start threshold is half the congestion window (min 2) */
u32 tcp_reno_ssthresh(struct sock *sk)
{
	const struct tcp_sock *tp = tcp_sk(sk);
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
	.init		= tcp_reno_abe_init,
	.undo_cwnd	= tcp_reno_undo_cwnd,
	.pkts_acked	= tcp_reno_abe_acked,
	.cwnd_event	= tcp_reno_abe_cwnd_event
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