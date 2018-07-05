function FileHandlerForTaskTime( filename, fout )
    inputfile = fopen(filename);
    if inputfile == -1
        return;
    end
    file_cache = 4096000;
    fseek(inputfile, 0, 'eof');
    filelength = ftell(inputfile);
    fseek(inputfile, 0, 'bof');
    page = floor(filelength / file_cache)+1;
    
    l = 0;
    time_max = 0;
    
    for i = 1:page
        [buf,size] = fread(inputfile, [1,file_cache],'uint8=>char');
        buf1(l+1:l+size) = buf(1:size);
        length_buf1 = l + size;
        block = strfind(buf1(1:length_buf1), 'DBG_INFO');
        m = length(block);
        if (m == 0)
            fclose(inputfile);
            return;
        end
        for j = 1:m-1
            buf2 = buf1(block(j):block(j+1)-1);
            time_block = strfind(buf2, 'TUse:');
            time1_block = strfind(buf2, 'ICI0:');
            time = fix(str2double(buf2(time_block + length('TUse:') : time1_block-2)));
            if (time > time_max)
                time_max = time;
            end
        end
        l = length_buf1 - block(m) + 1;
        buf1(1:l) = buf1(block(m):length_buf1);
    end
    fwrite(fout,[num2str(time_max) , '   '],'char');
    fwrite(fout,[filename, char(13), char(10)],'char');
    fclose(inputfile);
end

