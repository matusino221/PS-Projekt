reset
set term svg size 1024 720 dynamic
set output "../graphs/delays_size.svg"
set title "Delays of reveived packets"
set xlabel "Number of nodes"
set ylabel "Delay [ms]"

stats "delays_size.dat" u 2 #ta 2 je ktory stlpec sa ma použiť na rátanie

set label 1 gprintf("Mean = %g", STATS_mean) at 15, STATS_max #tu 15 si zmen podla toho kam chceš umestniť vyratane data
set label 2 gprintf("Standard deviation = %g", STATS_stddev) at 15, STATS_max-40
plot STATS_mean-STATS_stddev with filledcurves y1=STATS_mean lt 1 lc rgb "#b7e3e2" notitle,\
STATS_mean+STATS_stddev with filledcurves y1=STATS_mean lt 1 lc rgb "#b7e3e2" title "Standard deviation",\
STATS_mean w l lt 6 title "Mean",\
"delays_size.dat" u 1:2 title "Delays" w l lt 8 #tu 2 si zmen ked chceš ako Y vykresliť iný stlpec