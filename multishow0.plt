#==============================================================================
# GNUPLOT-palette (set1)
#------------------------------------------------------------------------------
# see more at https://github.com/Gnuplotting/gnuplot-palettes
#==============================================================================
# line styles for ColorBrewer Set1
# for use with qualitative/categorical data
# provides 8 easy-to-name colors
# compatible with gnuplot >=4.2
# author: Anna Schneider

# line styles
set style line 1 lt 1 lc rgb '#E41A1C' # red
set style line 2 lt 1 lc rgb '#377EB8' # blue
set style line 3 lt 1 lc rgb '#4DAF4A' # green
set style line 4 lt 1 lc rgb '#984EA3' # purple
set style line 5 lt 1 lc rgb '#FF7F00' # orange
set style line 6 lt 1 lc rgb '#FFFF33' # yellow
set style line 7 lt 1 lc rgb '#A65628' # brown
set style line 8 lt 1 lc rgb '#F781BF' # pink

# palette
set palette maxcolors 8
set palette defined ( 0 '#E41A1C',\
    	    	      1 '#377EB8',\
		      2 '#4DAF4A',\
		      3 '#984EA3',\
		      4 '#FF7F00',\
		      5 '#FFFF33',\
		      6 '#A65628',\
		      7 '#F781BF' )

#==============================================================================
# TERMINAL
#==============================================================================
set termoption enhanced
set terminal GNUTERM size 1152,648
set encoding utf8
#==============================================================================
# MULTIPLOT
#==============================================================================
set multiplot layout 1,2 rowsfirst downwards

#==============================================================================
# SETUP COMMANDS
#==============================================================================
#==============================================================================
# GNUPLOT-palette (paired)
#------------------------------------------------------------------------------
# see more at https://github.com/Gnuplotting/gnuplot-palettes
#==============================================================================
# line styles for ColorBrewer Paired
# for use with qualitative/categorical data
# provides 8 colors in 4 pairs
# compatible with gnuplot >=4.2
# author: Anna Schneider

# line styles
set style line 1 lt 1 lc rgb '#A6CEE3' # light blue
set style line 2 lt 1 lc rgb '#1F78B4' # dark blue
set style line 3 lt 1 lc rgb '#B2DF8A' # light green
set style line 4 lt 1 lc rgb '#33A02C' # dark green
set style line 5 lt 1 lc rgb '#FB9A99' # light red
set style line 6 lt 1 lc rgb '#E31A1C' # dark red
set style line 7 lt 1 lc rgb '#FDBF6F' # light orange
set style line 8 lt 1 lc rgb '#FF7F00' # dark orange

# palette
set palette maxcolors 8
set palette defined ( 0 '#A6CEE3',\
    	    	      1 '#1F78B4',\
		      2 '#B2DF8A',\
		      3 '#33A02C',\
		      4 '#FB9A99',\
		      5 '#E31A1C',\
		      6 '#FDBF6F',\
		      7 '#FF7F00' )

set xlabel 'time' enhanced textcolor '#404040'
set ylabel 'voltage' enhanced textcolor '#404040'

set border 3 front linetype 1 linewidth 2 linecolor '#404040'
unset grid
set style fill solid noborder
set style histogram
set tics border nomirror out scale 0.5,0.25 norotate enhanced textcolor '#404040' front
set xtics border nomirror out scale 0.5,0.25 norotate enhanced textcolor '#404040'
unset x2tics
set mxtics
unset mx2tics
set ytics border nomirror out scale 0.5,0.25 norotate enhanced textcolor '#404040'
unset y2tics
set mytics
unset my2tics
unset ztics
unset mztics
unset rtics
unset mrtics
unset key
set boxwidth 0.9 relative
set datafile missing "?"
#==============================================================================
# CUSTOM EXPLICIT GNUPLOT COMMANDS
#==============================================================================
set style data histogram
#==============================================================================
# PLOT COMMANDS
#==============================================================================
plot \
    'plot0.dat' index 0 with lines linestyle 1 linewidth 2
do for [i=1:20] { unset style line i }

#==============================================================================
# SETUP COMMANDS
#==============================================================================
#==============================================================================
# GNUPLOT-palette (paired)
#------------------------------------------------------------------------------
# see more at https://github.com/Gnuplotting/gnuplot-palettes
#==============================================================================
# line styles for ColorBrewer Paired
# for use with qualitative/categorical data
# provides 8 colors in 4 pairs
# compatible with gnuplot >=4.2
# author: Anna Schneider

# line styles
set style line 1 lt 1 lc rgb '#A6CEE3' # light blue
set style line 2 lt 1 lc rgb '#1F78B4' # dark blue
set style line 3 lt 1 lc rgb '#B2DF8A' # light green
set style line 4 lt 1 lc rgb '#33A02C' # dark green
set style line 5 lt 1 lc rgb '#FB9A99' # light red
set style line 6 lt 1 lc rgb '#E31A1C' # dark red
set style line 7 lt 1 lc rgb '#FDBF6F' # light orange
set style line 8 lt 1 lc rgb '#FF7F00' # dark orange

# palette
set palette maxcolors 8
set palette defined ( 0 '#A6CEE3',\
    	    	      1 '#1F78B4',\
		      2 '#B2DF8A',\
		      3 '#33A02C',\
		      4 '#FB9A99',\
		      5 '#E31A1C',\
		      6 '#FDBF6F',\
		      7 '#FF7F00' )

set xlabel 'time' enhanced textcolor '#404040'
set ylabel 'voltage' enhanced textcolor '#404040'

set border 3 front linetype 1 linewidth 2 linecolor '#404040'
unset grid
set style fill solid noborder
set style histogram
set tics border nomirror out scale 0.5,0.25 norotate enhanced textcolor '#404040' front
set xtics border nomirror out scale 0.5,0.25 norotate enhanced textcolor '#404040'
unset x2tics
set mxtics
unset mx2tics
set ytics border nomirror out scale 0.5,0.25 norotate enhanced textcolor '#404040'
unset y2tics
set mytics
unset my2tics
unset ztics
unset mztics
unset rtics
unset mrtics
unset key
set boxwidth 0.9 relative
set datafile missing "?"
#==============================================================================
# CUSTOM EXPLICIT GNUPLOT COMMANDS
#==============================================================================
set style data histogram
#==============================================================================
# PLOT COMMANDS
#==============================================================================
plot \
    'plot1.dat' index 0 with lines linestyle 1 linewidth 2
do for [i=1:20] { unset style line i }

unset multiplot

