import sys
import datetime
from fabric.api import env

#
# Fabric config
#

# User and password
env.user = 'root'
env.password = 'root'

# Set shell used to execute commands
env.shell = '/bin/sh -c'

# SSH connection timeout
env.timeout = 5

# Number of concurrent processes
env.pool_size = 10


#
# Testbed config
#

# Path to scripts
TPCONF_script_path = '/home/danny/teacup/teacup-1.1'
# DO NOT remove the following line
sys.path.append(TPCONF_script_path)

# Set debugging level (0 = no debugging info output) 
TPCONF_debug_level = 0

# Host lists
TPCONF_router = ['pi3router', ]
TPCONF_hosts = [ 'pi3host2', 'pi3host7', ]

# Map external IPs to internal IPs
TPCONF_host_internal_ip = {
    'pi3router': ['172.16.10.1', '172.16.20.1'],
    'pi3host2':  ['172.16.10.2'],
    'pi3host7':  ['172.16.20.7'],
}

#
# Experiment settings
#

# Maximum allowed time difference between machines in seconds
# otherwise experiment will abort cause synchronisation problems
TPCONF_max_time_diff = 2

# Experiment name prefix used if not set on the command line
# The command line setting will overrule this config setting
now = datetime.datetime.today()
# old default test ID prefix (version < 1.0)
#TPCONF_test_id = now.strftime("%Y%m%d-%H%M%S") + '_experiment'
# new default test ID prefix
TPCONF_test_id = 'exp_' + now.strftime("%Y%m%d-%H%M%S")

# Directory to store log files on remote host
TPCONF_remote_dir = '/tmp/'

# Time offset measurement options
# Enable broadcast ping on external/control interfaces
TPCONF_bc_ping_enable = '1'
# Specify rate of pings in packets/second
TPCONF_bc_ping_rate = 1
# Specify multicast address to use (must be broadcast or multicast address)
# If this is not specified, by default the ping will be send to the subnet
# broadcast address.
#TPCONF_bc_ping_address = '224.0.1.199'

# List of router queues/pipes

# Each entry is a tuple. The first value is the queue number and the second value
# is a comma separated list of parameters (see routersetup.py:init_pipe()).
# Queue numbers must be unique.

# Note that variable parameters must be either constants or or variable names
# defined by the experimenter. Variables are evaluated during runtime. Variable
# names must start with a 'V_'. Parameter names can only contain numbes, letter
# (upper and lower case), underscores (_), and hypen/minus (-).

# All variables must be defined in TPCONF_variable_list (see below).

# Note parameters must be configured appropriately for the router OS, e.g. there
# is no CoDel on FreeBSD; otherwise the experiment will abort witn an error.

TPCONF_router_queues = [
    # Set same delay for every host
    ('1', " source='172.16.10.0/24', dest='172.16.20.0/24', delay=V_delay, "
     " loss=V_loss, rate=V_up_rate, queue_disc=V_aqm, queue_size=V_bsize, "
     "queue_disc_params=V_aqm_params "),
    ('2', " source='172.16.20.0/24', dest='172.16.10.0/24', delay=V_delay, "
     " loss=V_loss, rate=V_down_rate, queue_disc=V_aqm, queue_size=V_bsize, "
     "queue_disc_params=V_aqm_params "),
]

# List of traffic generators

traffic_iperf = [
    # Specifying external addresses traffic will be created using the _first_
    # internal addresses (according to TPCONF_host_internal_ip)
    ('0.0', '1', " start_iperf, client='pi3host2', server='pi3host7', port=5000, "
     " duration=V_duration "),
    # ('0.0', '2', " start_iperf, client='pi3host7', server='pi3host2', port=5001, "
    #  " duration=V_duration "),
]

# THIS is the traffic generator setup we will use
TPCONF_traffic_gens = traffic_iperf

# Parameter ranges

# Duration in seconds
TPCONF_duration = 30

# Number of runs for each setting
TPCONF_runs = 1

# If '1' enable ecn for all hosts, if '0' disable ecn for all hosts
TPCONF_ECN = ['1']

# TCP congestion control algorithm used
# Possible algos are: default, host<N>, newreno, cubic, cdg, hd, htcp, compound, vegas
# Note that the algo support is OS specific, so must ensure the right OS is booted
# Windows: newreno (default), compound
# FreeBSD: newreno (default), cubic, hd, htcp, cdg, vegas
# Linux: newreno, cubic (default), htcp, vegas
# If you specify 'default' the default algorithm depending on the OS will be used
# If you specify 'host<N>' where <N> is an integer starting from 0 to then the
# algorithm will be the N-th algorithm specified for the host in TPCONF_host_TCP_algos 
# (in case <N> is larger then the number of algorithms specified, it is set to 0
TPCONF_TCP_algos = ['newreno', ]

# Specify TCP congestion control algorithms used on each host
# TPCONF_host_TCP_algos = {
#     'newtcp1': ['default', 'newreno', ],
#     'newtcp2': ['default', 'newreno', ],
#     'newtcp3': ['default', 'newreno', ],
#     'newtcp4': ['default', 'compound', ],
# }

# Specify TCP parameters for each host and each TCP congestion control algorithm
# Each parameter is of the form <sysctl name> = <value> where <value> can be a constant
# or a V_ variable
TPCONF_host_TCP_algo_params = {
    # 'newtcp1': {'cdg': ['net.inet.tcp.cc.cdg.beta_delay = V_cdg_beta_delay',
    #                     'net.inet.tcp.cc.cdg.beta_loss = V_cdg_beta_loss',
    #                     'net.inet.tcp.cc.cdg.exp_backoff_scale = 3',
    #                     'net.inet.tcp.cc.cdg.smoothing_factor = 8',
    #                     'net.inet.tcp.cc.cdg.loss_compete_consec_cong = 5',
    #                     'net.inet.tcp.cc.cdg.loss_compete_hold_backoff = 5',
    #                     'net.inet.tcp.cc.cdg.alpha_inc = 0'],
    #             },
}

# Specify arbitray commands that are executed on a host at the end of the host 
# intialisation (after general host setup, ecn and tcp setup). The commands are
# executed in the shell as written after any V_ variables have been replaced.
# LIMITATION: only one V_ variable per command
TPCONF_host_init_custom_cmds = {
    'pi3host2' : [ 'sysctl net.inet.tcp.cc.abe=1' ],
    'pi3host7' : [ 'sysctl net.inet.tcp.cc.abe=1' ]
}

# Delays in ms
TPCONF_delays = [100]

# Loss rates in percent
TPCONF_loss_rates = [0]

# Bandwidth (downstream, upstream)
# Note: Linux syntax
TPCONF_bandwidths = [
    ('10mbit', '10mbit'),
]

# AQM
# Note this is router OS specific
# Linux: fifo (mapped to pfifo), pfifo, bfifo, fq_codel, codel, pie, red, ...
#        (see tc man page for full list)
# FreeBSD: fifo, red
TPCONF_aqms = ['pie', ]

# AQM parameters
# example for PIE: 'ecn target 20ms tupdate 30ms'
# check manual for more: man tc-pie, tc-fq_codel...
TPCONF_aqms_params = ['ecn target 20ms tupdate 30ms', ]

# Buffer size
# If router is Linux this is mostly in packets/slots, but it depends on AQM
# (e.g. for bfifo it's bytes)
# If router is FreeBSD this would be in slots by default, but we can specify byte sizes
# (e.g. we can specify 4Kbytes)
TPCONF_buffer_sizes = [64]

# CDG parameters
# beta delay
#TPCONF_cdg_beta_delay_facs = [70, 50, 90]
# beta loss
#TPCONF_cdg_beta_loss_facs = [50, 30, 70]


# List of all parameters that can be varied

# The key of each item is the identifier that can be used in TPCONF_vary_parameters
# (see below).
# The value of each item is a 4-tuple. First, a list of variable names.
# Second, a list of short names uses for the file names.
# For each parameter varied a string '_<short_name>_<value>' is appended to the log
# file names (appended to chosen prefix). Note, short names should only be letters
# from a-z or A-Z. Do not use underscores or hyphens!
# Third, the list of parameters values. If there is more than one variable this must
# be a list of tuples, each tuple having the same number of items as teh number of
# variables. Fourth, an optional dictionary with additional variables, where the keys
# are the variable names and the values are the variable values.

TPCONF_parameter_list = {
#   Vary name		V_ variable	            file name	    values      extra vars
    'ecns' 	        :  (['V_ecn'],	        ['ecn'], 	    TPCONF_ECN, {}),
    'delays' 	    :  (['V_delay'], 	    ['del'], 	    TPCONF_delays, {}),
    'loss'  	    :  (['V_loss'], 	    ['loss'],   	TPCONF_loss_rates, {}),
    'tcpalgos' 	    :  (['V_tcp_cc_algo'],  ['tcp'], 	    TPCONF_TCP_algos, {}),
    'aqms'	        :  (['V_aqm'], 	        ['aqm'], 	    TPCONF_aqms, {}),
    'aqms_params'   :  (['V_aqm_params'],   ['aqm_params'], TPCONF_aqms_params, {}),
    'bsizes'	    :  (['V_bsize'], 	    ['bs'], 	    TPCONF_buffer_sizes, {}),
    'runs'	        :  (['V_runs'],         ['run'], 	    range(TPCONF_runs), {}),
    'bandwidths'    :  (['V_down_rate', 'V_up_rate'], ['down', 'up'], TPCONF_bandwidths, {}),
    # 'cdg_beta_dels' :  (['V_cdg_beta_delay'], ['cdgbdel'], TPCONF_cdg_beta_delay_facs, {}),
    # 'cdg_beta_loss' :  (['V_cdg_beta_loss'], ['cdgblo'],   TPCONF_cdg_beta_loss_facs,  {}),
}

# Default setting for variables (used for variables if not varied)

# The key of each item is the parameter  name. The value of each item is the default
# parameter value used if the variable is not varied.

TPCONF_variable_defaults = {
#   V_ variable			    value
    'V_ecn'  		    :	TPCONF_ECN[0],
    'V_duration'  	    :	TPCONF_duration,
    'V_delay'  		    :	TPCONF_delays[0],
    'V_loss'   		    :	TPCONF_loss_rates[0],
    'V_tcp_cc_algo' 	:	TPCONF_TCP_algos[0],
    'V_down_rate'   	:	TPCONF_bandwidths[0][0],
    'V_up_rate'	    	:	TPCONF_bandwidths[0][1],
    'V_aqm'	    	    :	TPCONF_aqms[0],
    'V_aqm_params'      :   TPCONF_aqms_params[0],
    'V_bsize'	    	:	TPCONF_buffer_sizes[0],
    #'V_cdg_beta_delay'	: 	TPCONF_cdg_beta_delay_facs[0],
    #'V_cdg_beta_loss'	: 	TPCONF_cdg_beta_loss_facs[0],
    'V_test'	    	:	'foobar',
}

# Specify the parameters we vary through all values, all others will be fixed
# according to TPCONF_variable_defaults
TPCONF_vary_parameters = ['tcpalgos', 'delays', 'loss', 'bandwidths',
                          'aqms', 'aqms_params', 'bsizes', 'runs', ]
