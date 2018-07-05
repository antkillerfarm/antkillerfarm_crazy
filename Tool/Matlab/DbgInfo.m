function DbgInfo()
    %main function
    [filename, pathname] = uigetfile('*.*');
    if filename == 0
        return;
    end
    fullname = [pathname, filename];
    inputfile = fopen(fullname);
    if inputfile == -1
        return;
    end
    outputfile(1) = fopen([pathname,'X.txt'], 'w');
    file_cache = 4096;
    fseek(inputfile, 0, 'eof');
    filelength = ftell(inputfile);
    fseek(inputfile, 0, 'bof');
    page = floor(filelength / file_cache)+1;

    percent = 0
    pos = zeros(1,2);
    l = 0;

    for i = 1:page
        [buf,size] = fread(inputfile, [1,file_cache],'uint8=>char');
        buf1(l+1:l+size) = buf(1:size);
        length_buf1 = l + size;
        block = strfind(buf1(1:length_buf1), 'DBG_INFO');
        m = length(block);
        for j = 1:m-1
            buf2 = buf1(block(j):block(j+1)-1);
            n = length(buf2);
            index = 1;
            for k = length('DBG_INFO'):n
                if (strcmp(buf2(k),'	'))
                    pos(index) = k;
                    if (index == 2)
                        break;
                    end
                    index = index + 1;
                end
            end
            ex_cnt = fix(str2double(buf2(pos(1):pos(2))));
            sub_block = strfind(buf2, 'rst_ch:');
            for k = sub_block+length('rst_ch:'):n
                if (~(strcmp(buf2(k), char(10)) || strcmp(buf2(k), char(13))))
                    s = sprintf('%d\r\n',ex_cnt);
                    s = [s,buf2(sub_block+length('rst_ch:')+2:n)];
                    fwrite(outputfile(1),s,'char');
                    break;
                end
            end
        end
        percent = i/page*100
        l = length_buf1 - block(m) + 1;
        buf1(1:l) = buf1(block(m):length_buf1);
    end
    fclose('all');
end