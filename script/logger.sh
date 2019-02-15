#!/bin/sh
PARA=$1
CONFIG="./config/server.xml"


ASLOG=`grep AS.log $CONFIG | sed -e 's/<[a-zA-Z]*>//' | sed -e 's/<\/[a-zA-Z]*>//'`
LSLOG=`grep LS.log $CONFIG | sed -e 's/<[a-zA-Z]*>//' | sed -e 's/<\/[a-zA-Z]*>//'`
GW1LOG=`grep GW1.log $CONFIG | sed -e 's/<[a-zA-Z]*>//' | sed -e 's/<\/[a-zA-Z]*>//'`
GW2LOG=`grep GW2.log $CONFIG | sed -e 's/<[a-zA-Z]*>//' | sed -e 's/<\/[a-zA-Z]*>//'`
GS1LOG=`grep GS1.log $CONFIG | sed -e 's/<[a-zA-Z]*>//' | sed -e 's/<\/[a-zA-Z]*>//'`
ALL=$ASLOG" "$LSLOG" "$GW1LOG" "$GW2LOG" "$GS1LOG


clear

case  $PARA in
	as)
	tail --follow=name --retry $ASLOG --max-unchanged-stats=3 -n 40 -q
	;;
	ls)
	tail --follow=name --retry $LSLOG --max-unchanged-stats=3 -n 40 -q
	;;
	gw)
	tail --follow=name --retry $GW1LOG $GW2LOG --max-unchanged-stats=3 -n 40 -q
	;;
	gs)
	tail --follow=name --retry $GS1LOG --max-unchanged-stats=3 -n 40 -q
	;;
	*)
	tail --follow=name --retry $ALL --max-unchanged-stats=3 -n 40 -q
	;;
esac
