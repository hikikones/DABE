#include <linux/module.h>
#include <net/tcp.h>

/*
 * TCP Reno congestion control
 * This is special case used for fallback as well.
 */
/* This is Jacobson's slow start and congestion avoidance.
 * SIGCOMM '88, p. 328.
 */

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

	int used_beta;

	if (sock_net(sk)->ipv4.sysctl_tcp_ecn > 0) {
		/* if ECN enabled and we see ECE */
		if (tp->snd_cwnd < tp->snd_ssthresh)
		used_beta = abe_ss_beta;
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
	.undo_cwnd	= tcp_reno_undo_cwnd,
};

static int __init renotcp_register(void)
{
	printk("\n\nHelloooooooooooooooooo!!\n\n");
	return tcp_register_congestion_control(&tcp_reno);
}

static void __exit renotcp_unregister(void)
{
	tcp_unregister_congestion_control(&tcp_reno);
}

module_init(renotcp_register);
module_exit(renotcp_unregister);

MODULE_AUTHOR("Danny");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("RENO TCP ABE");
MODULE_VERSION("2.3");