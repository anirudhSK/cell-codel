#!/bin/bash

exec /home/ubuntu/cell-codel/codel-for-cellsim/cell-sched /home/ubuntu/verizon_lte_uplink_slice.ms /home/ubuntu/verizon_lte_downlink_slice.ms 00:00:00:00:00:02 0 LTE-eth0 LTE-eth1 >/tmp/cellsim-stdout 2>/tmp/cellsim-stderr
