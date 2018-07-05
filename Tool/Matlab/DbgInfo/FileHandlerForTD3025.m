function FileHandlerForTD3025( filename, fout )
    inputfile = fopen(filename);
    if inputfile == -1
        return;
    end
    file_cache = 4096000;
    fseek(inputfile, 0, 'eof');
    filelength = ftell(inputfile);
    fseek(inputfile, 0, 'bof');
    page = floor(filelength / file_cache)+1;
    
    pos = 0;
    l = 0;
    bit_sync = zeros(1,48);
    filename_print_flag = 0;
    
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
            n = length(buf2);
            for k = length('DBG_INFO')+2:n
                if (strcmp(buf2(k),'	'))
                    pos = k;
                    break;
                end
            end
            ex_cnt = fix(str2double(buf2(length('DBG_INFO')+2:pos)));
            trk_block = strfind(buf2, 'TRKInfo:');
            if (isempty(trk_block))
            	continue;
            end
            pvt_block = strfind(buf2, 'PVTInfo:');
            if (isempty(pvt_block))
            	continue;
            end
            val_change_num = 0;
            sub_block = strfind(buf2(trk_block : pvt_block-2), [char(13), char(10)]);
            lines = length(sub_block);
            if (lines <= 2)
                continue;
            end
            for k = 3:lines
                if (k == lines)
                    buf3 = buf2(trk_block + sub_block(k) + 1 : pvt_block - 2);
                else
                    buf3 = buf2(trk_block + sub_block(k) + 1 : trk_block + sub_block(k + 1));
                end
                channel = fix(str2double(buf3(1 : 2)));
                trk_type = fix(str2double(buf3(4 : 5)));
                s_block = strfind(buf3, '/');
                s_lenth = length(s_block);
                if (s_lenth < 6)
                    break;
                end
                sv_type = fix(str2double(buf3(s_block(1)-1:s_block(1)-1)));
                prn = fix(str2double(buf3(s_block(2)+1:s_block(2)+2)));
                val = bitget(hex2dec(buf3(s_block(6) + 14 : s_block(6) + 17)), 5);
                if (val == 0 && bit_sync(channel + 1) == 1 && ((sv_type == 2 && prn > 5) || sv_type == 1) && trk_type == 2)
                    val_change_num = val_change_num + 1;
                end
                bit_sync(channel + 1) = val;
            end
            if (val_change_num >= 3)
                if (filename_print_flag == 0)
                    filename_print_flag = 1;
                    fwrite(fout,[filename, char(13), char(10)],'char');
                end
                fwrite(fout,['DBG_INFO ', num2str(ex_cnt) , char(13), char(10)],'char');
            end
        end
        l = length_buf1 - block(m) + 1;
        buf1(1:l) = buf1(block(m):length_buf1);
    end
    fclose(inputfile);
end

