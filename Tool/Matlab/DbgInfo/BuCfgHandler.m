function BuCfgHandler()
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
    l = 0;
    pos = 0;
    bu_num = 0;

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
        for j = 1 : m - 1
            buf2 = buf1(block(j) : block(j + 1) - 1);
            n = length(buf2);
            for k = length('DBG_INFO') + 2 : n
                if (strcmp(buf2(k),'	'))
                    pos = k;
                    break;
                end
            end
            ex_cnt = fix(str2double(buf2(length('DBG_INFO')+2:pos)));
            tim_block = strfind(buf2, 'tim:');
            if (isempty(tim_block))
            	fclose(inputfile);
                return;
            end
            sub_block = strfind(buf2(tim_block:n), [char(13), char(10)]);
            lines = length(sub_block);
            if (lines == 0)
                fclose(inputfile);
                return;
            end
            buf3 = buf2(tim_block + 4 : tim_block + sub_block(1) - 1);
            bu_block = strfind(buf3, '/');
            if (isempty(bu_block))
            	fclose(inputfile);
                return;
            end
            bu_num = bu_num + 1;
            bu_value(bu_num) = str2double(buf3(bu_block(3) + 1 : bu_block(4) - 1));
            ex_cnt1(bu_num) = ex_cnt;
        end
        l = length_buf1 - block(m) + 1;
        buf1(1:l) = buf1(block(m):length_buf1);
    end
    bu_value2 = bu_value(2 : end) - bu_value(1 : end - 1);
    bu_value_len = length(bu_value);
    bu_abs_value = 0;
    for j = 1 : bu_value_len
        bu_abs_value = bu_abs_value + abs(bu_value(j));
    end
    bu_abs_value1 = bu_abs_value / bu_value_len;
    fclose('all');
end

