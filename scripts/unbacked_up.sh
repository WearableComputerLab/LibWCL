UNBACKED_UP_FILES=unbacked_up_files.txt

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
      echo "put $i" | sftp snappybackup@wcl.ml.unisa.edu.au:$BACKUP_DIRECTORY

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

