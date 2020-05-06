#!/bin/bash
echo "Bash version ${BASH_VERSION}"

if [ $# -ne 1 ]
	then
		echo "$0: usage: zadanieps2.sh <output dir>"
		exit 1
fi

echo "Preparing..."
mkdir $1
mkdir $1/data
mkdir $1/netanim
mkdir $1/flowmon
mkdir $1/pcap
mkdir $1/graphs


> $1/data/delays_nodes.dat
> $1/data/delays_size.dat
> $1/data/lossRates_size.dat

#change number of nodes
echo "Simulations start"
nodes=10
while [ $nodes -le 20 ]
	do
		./waf --run "zadanieps2 --nodes=$nodes --outdir=$1 --sdelaysnodes=true --workmode=nodes"
		nodes=$(($nodes + 5))
	done

#change size of packet
packet=128
while [ $packet -le 1024 ]
	do
		./waf --run "zadanieps2 --nodes=16 --packet=$packet --outdir=$1 --sdelayssize=true --slosssize=true --workmode=size"
		packet=$(($packet + 32))
	done
echo "Simulations ended"

#prepare data for gnuplot
echo "Preparing data for plotting"
./scripts/prepare_data $1'/data/delays_nodes.dat' 'mean'
./scripts/prepare_data $1'/data/delays_size.dat' 'mean'
./scripts/prepare_data $1'/data/lossRates_size.dat' 'mean'

origin=$(pwd)

cp ./scripts/plotting1.gnuplot $1/data/plotting1.gnuplot
cp ./scripts/plotting2.gnuplot $1/data/plotting2.gnuplot
cp ./scripts/plotting3.gnuplot $1/data/plotting3.gnuplot
cd $1/data
#gnu plot
echo "Plotting data"
gnuplot plotting1.gnuplot
gnuplot plotting2.gnuplot
gnuplot plotting3.gnuplot

#cleanup
rm plotting1.gnuplot
rm plotting2.gnuplot
rm plotting3.gnuplot

cd $origin

echo "All done"