function BatDbgHandler()
    pathname = uigetdir('E:\my\test\');
    %pathname = uigetdir();
    if (pathname == 0)
        return;
    end
    fileout = fopen([pathname,'\result1.txt'], 'w');
    if (fileout == 0)
        return;
    end
    DirHandler(pathname, fileout);
    fclose(fileout);
end

