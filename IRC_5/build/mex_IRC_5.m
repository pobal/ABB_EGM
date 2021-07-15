finishup='';
if(~bdIsLoaded('library_IRC_5'))
  load_system('library_IRC_5');
  finishup=onCleanup(@()close_system('library_IRC_5',0));
end
blkHandle=get_param('library_IRC_5/IRC_5','Handle');
appdata=sfunctionwizard(blkHandle,'GetApplicationData');
appdata=sfunctionwizard(blkHandle,'Build',appdata);

