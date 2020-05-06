#Tento skript vykresluje pre každý jedne flow zlásť graf
#jenoduchá uprava indexov stlpcov zmeni ake data sa budu počítať
#POZOR stlpce sú indexované od 1

#Flow 1
reset
set term svg size 1024 720 dynamic
set output "../graphs/useful-data-flow1.svg"
set title "Ratio of useful and all transfered data for Flow 1"
set xlabel "Packet size [B]"
set ylabel "Useful data [%]"

stats 'useful-data.dat' u 2

set label 1 gprintf("Mean = %g", STATS_mean) at 160, STATS_min+1
set label 2 gprintf("Standard deviation = %g", STATS_stddev) at 160, STATS_min+0.7
plot STATS_mean-STATS_stddev with filledcurves y1=STATS_mean lt 1 lc rgb "#b7e3e2" notitle,\
STATS_mean+STATS_stddev with filledcurves y1=STATS_mean lt 1 lc rgb "#b7e3e2" title "Standard deviation",\
STATS_mean w l lt 6 title "Mean",\
"useful-data.dat" u 1:2 title "Useful data" w l lt 8


#Flow 2
reset
set term svg size 1024 720 dynamic
set output "../graphs/useful-data-flow2.svg"
set title "Ratio of useful and all transfered data for Flow 2"
set xlabel "Packet size [B]"
set ylabel "Useful data [%]"

stats 'useful-data.dat' u 3

set label 1 gprintf("Mean = %g", STATS_mean) at 160, STATS_min+1
set label 2 gprintf("Standard deviation = %g", STATS_stddev) at 160, STATS_min+0.7
plot STATS_mean-STATS_stddev with filledcurves y1=STATS_mean lt 1 lc rgb "#b7e3e2" notitle,\
STATS_mean+STATS_stddev with filledcurves y1=STATS_mean lt 1 lc rgb "#b7e3e2" title "Standard deviation",\
STATS_mean w l lt 6 title "Mean",\
"useful-data.dat" u 1:3 title "Useful data" w l lt 8



#Flow 3
reset
set term svg size 1024 720 dynamic
set output "../graphs/useful-data-flow3.svg"
set title "Ratio of useful and all transfered data for Flow 3"
set xlabel "Packet size [B]"
set ylabel "Useful data [%]"

stats 'useful-data.dat' u 4

set label 1 gprintf("Mean = %g", STATS_mean) at 160, STATS_min+1
set label 2 gprintf("Standard deviation = %g", STATS_stddev) at 160, STATS_min+0.7
plot STATS_mean-STATS_stddev with filledcurves y1=STATS_mean lt 1 lc rgb "#b7e3e2" notitle,\
STATS_mean+STATS_stddev with filledcurves y1=STATS_mean lt 1 lc rgb "#b7e3e2" title "Standard deviation",\
STATS_mean w l lt 6 title "Mean",\
"useful-data.dat" u 1:4 title "Useful data" w l lt 8



#Flow 4
reset
set term svg size 1024 720 dynamic
set output "../graphs/useful-data-flow4.svg"
set title "Ratio of useful and all transfered data for Flow 4"
set xlabel "Packet size [B]"
set ylabel "Useful data [%]"

stats 'useful-data.dat' u 5

set label 1 gprintf("Mean = %g", STATS_mean) at 160, STATS_min+1
set label 2 gprintf("Standard deviation = %g", STATS_stddev) at 160, STATS_min+0.7
plot STATS_mean-STATS_stddev with filledcurves y1=STATS_mean lt 1 lc rgb "#b7e3e2" notitle,\
STATS_mean+STATS_stddev with filledcurves y1=STATS_mean lt 1 lc rgb "#b7e3e2" title "Standard deviation",\
STATS_mean w l lt 6 title "Mean",\
"useful-data.dat" u 1:5 title "Useful data" w l lt 8



#Flow 5
reset
set term svg size 1024 720 dynamic
set output "../graphs/useful-data-flow5.svg"
set title "Ratio of useful and all transfered data for Flow 5"
set xlabel "Packet size [B]"
set ylabel "Useful data [%]"

stats 'useful-data.dat' u 6

set label 1 gprintf("Mean = %g", STATS_mean) at 160, STATS_min+1
set label 2 gprintf("Standard deviation = %g", STATS_stddev) at 160, STATS_min+0.7
plot STATS_mean-STATS_stddev with filledcurves y1=STATS_mean lt 1 lc rgb "#b7e3e2" notitle,\
STATS_mean+STATS_stddev with filledcurves y1=STATS_mean lt 1 lc rgb "#b7e3e2" title "Standard deviation",\
STATS_mean w l lt 6 title "Mean",\
"useful-data.dat" u 1:6 title "Useful data" w l lt 8
