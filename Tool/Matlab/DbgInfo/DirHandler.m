function DirHandler(pathname, fout)
    filelist = dir(pathname);
    filecnt = size(filelist);

    for i = 3 : filecnt(1)
        file_handled = i-2
        if (filelist(i).isdir)
            DirHandler([pathname,'\',filelist(i).name], fout);
        else
            FileHandlerForTD3025([pathname,'\',filelist(i).name], fout);
        end
    end
end

