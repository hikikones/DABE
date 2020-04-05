#include <linux/module.h>
#include <net/tcp.h>

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

	return max(tp->snd_cwnd >> 1U, 2U);
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