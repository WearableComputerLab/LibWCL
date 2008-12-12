UNBACKED_UP_FILES=unbacked_up_files.txt
REMOTE_ACCOUNT=snappybackup@wcl.ml.unisa.edu.au

# backup outstanding image files.
function backupFiles {

  for i in `cat $UNBACKED_UP_FILES`
  do 
    BACKUP_DIRECTORY=`echo $i | grep -oE "images/[0-9][0-9][0-9][0-9]/[0-9][0-9]/[0-9][0-9]"`

    # check to make sure that the image file exists. If the file doesn't exist
    # the problem is silently ignored because if there is no file it cannot be
    # backed up
    if [ -f $i ]
    then

      # grab the date details from the directory structure
      YEAR=`echo $BACKUP_DIRECTORY | awk -F'/' '{print $2}'`
      MONTH=`echo $BACKUP_DIRECTORY | awk -F'/' '{print $3}'`
      DAY=`echo $BACKUP_DIRECTORY | awk -F'/' '{print $4}'`

      echo "year:$YEAR month:$MONTH day:$DAY"

      # attempt to create the directories on WCL
      echo "mkdir images/$YEAR" | sftp $REMOTE_ACCOUNT
      echo "mkdir images/$YEAR/$MONTH" | sftp $REMOTE_ACCOUNT
      echo "mkdir images/$YEAR/$MONTH/$DAY" | sftp $REMOTE_ACCOUNT

      echo "put $i" | sftp $REMOTE_ACCOUNT:$BACKUP_DIRECTORY

      # find out how the sftp went.
      if [ $? = 1 ]
      then
        return 1
      fi
    fi
  done

  return 0 
}

# check to see if there are any outstanding file to be backed up
function checkBackups {

#check to see if there is a list of files to backup
if [ -f $UNBACKED_UP_FILES ]
then
  # attempt to backup the images listed in the backup file.
  backupFiles

  # check to see if the backup went smoothly
  if [ $? = 0 ]
  then

    # if everything went okay remove the list of files to backup 
    rm -rf $UNBACKED_UP_FILES
  else
    echo "Failed to sftp all files. Will try again later."
  fi
fi
}

checkBackups

