# Setting variables (loaded by using the "init" function):
#SETTINGS_FOLDER
#SETTINGS_FILE
#SERVER
#THIS
#SERVER_BASE
#THIS_BASE

# include global functions
. ./sg_check.global

#variables
SRC=""
DEST=""

aDONT_COPY=""


#
# 1. reading options and translate them to options for rsync:
# options for this script
# default options:
#	-u : only copy if source is newer
#	-avz : archive mode, verbos, compress
# -destructive means:
# 	--delete : delete files at target, if they don't exist at the source
#	-avz : archieve mode, verbose, compress
#
# # rsync options:
# -v : verbose
# -z : enable compression
# -a : recursive(-r), preserve permission,timestampt,owner and group
# -e : use remote shell (ssh, in this case)
# -i : display changes between source and destination
# -u : don't copy files, if they are newer on the destination
# --delete : delete files at target, if they don't exist at the source
# --existing : don't create new files at destination, if they don't exist there yet
# --progress : show progress during transfer
# -n : don't make any changes, just simulate. (dry run). output will be the same.
RSYNC_OPTIONS="-a --fuzzy -u --modify-window=2"
PRINT_CMD=0
if [ $# -gt 0 ]; then
	while [ "$1" != "" ]; do
		#echo "Parameter count: $#"
		if [ $# -eq 2 ]; then
			SRC="$1"
		elif [ $# -eq 1 ]; then
			DEST="$1"
		else
			case "$1" in
				"--settings" )
					shift
					settingsDir="$1"
					;;
				"-printcmd" )
					PRINT_CMD=1
					;;
				"-destructive")
					#echo "settings options for update!"
					RSYNC_OPTIONS="$RSYNC_OPTIONS"" --delete"
					;;
				"-simulate" )
					RSYNC_OPTIONS="$RSYNC_OPTIONS"" -n"
					;;
				"-checksum" )
					RSYNC_OPTIONS="$RSYNC_OPTIONS"" -c --ignore-times"
					;;
				"-list" )
					RSYNC_OPTIONS="$RSYNC_OPTIONS"" --out-format=""%i%n"
					;;
				"-verbose" )
					RSYNC_OPTIONS="$RSYNC_OPTIONS"" -v --progress"
					;;
				"-max-depth" )
					RSYNC_OPTIONS="$RSYNC_OPTIONS"" --exclude=/*/*/*/*"
					;;
				*) echo "\"$1\": option not found!"
			esac
		fi
		shift
	done
fi
if [ "$settingsDir" != "" ]; then
	setSettingsFolder "$settingsDir"
	#if [ $? != 0 ]; then
		#echo "settings dir not found!"
		#exit 1
	#fi
fi

# load settings:
init
if [ $? != 0 ]; then
	echo "settings file not found!"
	exit 1
fi
if [ "$SERVER" != "" ] || [ "$THIS" != "" ] ; then
	RSYNC_OPTIONS="$RSYNC_OPTIONS -z -e ssh"
fi


#
# 2. invoking rsync:
#
#if [ "$(echo $SRC | sed s/://g)" != "$SRC" ]; then
	#SRC=$(echo $SRC | sed 's/:/:\"/g')\"
	#$RSYNC_OPTIONS="$RSYNC_OPTIONS -e ssh"
#fi
# echo "SRC: \"$SRC\",   DEST: $DEST"
if [ $PRINT_CMD != 0 ]; then
	echo "executing: " "'rsync $RSYNC_OPTIONS $SRC $DEST'"
fi
rsync $RSYNC_OPTIONS "$SRC" "$DEST"
