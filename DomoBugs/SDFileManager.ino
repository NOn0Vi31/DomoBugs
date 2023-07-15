///////////////////////////////////////////////////// initialize SD card
int SDCardInitialization()
{
  // Initialize the SD.
  if (!sd.begin(SD_CONFIG)) {
    sd.initErrorHalt(&Serial);
    return 0;    // init failed
  }
  else
    return 1; // init done   
}
/////////////////////////////////////////////////////
int ShowExistingFiles()
{
  // Open root directory
  if (!dir.open("/")){
    Serial.println(F("\tdir.open failed"));
    return 0;
  }
  Serial.println(F("\tdir open"));
  
  //sd.ls("/", LS_R);
  
  // Open next file in root.
  // Warning, openNext starts at the current position of dir so a
  // rewind may be necessary in your application.
  dir.rewind();
  while (file.openNext(&dir, O_RDONLY)) {
    file.printFileSize(&Serial);
    Serial.write(' ');
    file.printModifyDateTime(&Serial);
    Serial.write(' ');
    file.printName(&Serial);
    if (file.isDir()) {
      // Indicate a directory.
      Serial.write('/');
    }
    Serial.println();
    file.close();
  }
  if (dir.getError()) {
    return 0;
  } else {
    return 1;
  }
}
///////////////////////////////////////////////////// check file exist on SD card
int CheckFileExist(String filename)
{
    // check if file exist
    if (!sd.exists(filename))
        return 0;  // can't find index file
    return 1;
}
///////////////////////////////////////////////////// check file exist on SD card
void LoadFileFromSDCard(String filename, File32 *FileContent)
{
  //String FileType = filename.substring(filename.lastIndexOf('/')+1);
  //if(FileType.length()>3)
  //  filename = filename.substring(0,filename.length()-1);
  
  *FileContent = sd.open(filename);
}
//////////////////////////////////////////////////// create file
void CreateFile(String filename, File32 *FileContent)
{
  *FileContent = sd.open(filename, FILE_WRITE);
}