% This function is used to return blockset root folder across different platforms.
function blocksetroot = blocksetroot()
try
proj = simulinkproject;
blocksetroot=proj.RootFolder;
catch ex
blocksetroot='';
end
end
