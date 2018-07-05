function BuHandler()
    [filename, pathname] = uigetfile('*.*');
    if filename == 0
        return;
    end
    fullname = [pathname, filename];
    inputfile = fopen(fullname);
    if inputfile == -1
        return;
    end

    file_cache = 409600;
    fseek(inputfile, 0, 'eof');
    filelength = ftell(inputfile);
    fseek(inputfile, 0, 'bof');
    page = floor(filelength / file_cache)+1;
    bu_num = zeros(1,4);
    bu_all_num = 0;
    l = 0;

    for i = 1:page
        [buf,size] = fread(inputfile, [1, file_cache],'uint8=>char');
        buf1(l + 1 : l + size) = buf(1 : size);
        length_buf1 = l + size;
        block = strfind(buf1(1 : length_buf1), [char(13), char(10)]);
        m = length(block);
        for j = 0 : m - 1
            if (j == 0)
                buf2 = buf1(1 : block(j + 1) - 1);
            else
                buf2 = buf1(block(j) + 2: block(j + 1) - 1);
            end
            ch = fix(str2double(buf2(3))) + 1;
            bu_num(ch) = bu_num(ch) + 1;
            bu_all_num = bu_all_num + 1;
            if (strcmp(buf2(5), 'a'))
                flag = 1;
            else
                flag = -1;
            end
            value = hex2dec(buf2(6 : 12));
            if (abs(value) < 1000)
                bu(ch, bu_num(ch)) = flag * value * 5;
            end
        end
        percent = i/page*100
        l = length_buf1 - block(m) + 1 - 2;
        buf1(1:l) = buf1(block(m) + 2 : length_buf1);
    end
    fclose('all');
end

