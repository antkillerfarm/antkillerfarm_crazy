function FileHandler1( filename, fout )
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
    fixed = 0;
    buf3 = '';
    out_flag = 0;
    
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
            index = 1;
            for k = length('DBG_INFO')+2:n
                if (strcmp(buf2(k),'	'))
                    pos = k;
                    break;
                end
            end
            ex_cnt = fix(str2double(buf2(length('DBG_INFO')+2:pos)));
            fix_block = strfind(buf2, 'fix: 3/');
            if ((~isempty(fix_block)) && (fixed == 0))
                fixed = ex_cnt;
            end
            if ((~isempty(fix_block)) && (fixed ~= 0) && (fixed == ex_cnt - 1))
                fix_block = strfind(buf2, 'fix:');
                sub_block = strfind(buf2(fix_block:n), [char(13), char(10)]);
                buf3 = buf2(fix_block + sub_block(2)+1: fix_block + sub_block(4)-2);
            end
            gga_block = strfind(buf2, 'GGA');
            if (~isempty(gga_block))
                if (ex_cnt < 120)
                    fclose(inputfile);
                    return;
                end
                out_flag = 1;
                fwrite(fout,[filename, char(13), char(10)],'char');
                fwrite(fout,['FIX:DBG_INFO ', num2str(fixed) , char(13), char(10)],'char');
                if (~isempty(buf3))
                    fwrite(fout,[buf3 , char(13), char(10)],'char');
                end
                fwrite(fout,['GGA:DBG_INFO ', num2str(ex_cnt) , char(13), char(10)],'char');
                fclose(inputfile);
                return;
            end
        end
        l = length_buf1 - block(m) + 1;
        buf1(1:l) = buf1(block(m):length_buf1);
    end
%     if (out_flag == 0)
%        fwrite(fout,[filename, char(13), char(10)],'char');
%        fwrite(fout,['FIX:DBG_INFO ', num2str(fixed) , char(13), char(10)],'char');
%        if (~isempty(buf3))
%            fwrite(fout,[buf3 , char(13), char(10)],'char');
%        end
%     end
    fclose(inputfile);
end

