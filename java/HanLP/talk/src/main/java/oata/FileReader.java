package oata;

import java.io.*;
import java.util.ArrayList;
import java.util.List;

public class FileReader {
	public static final String FILE_SEPARATOR = System.getProperty("file.separator");

	public static String getFile_charset() {
		return file_charset;
	}

	public static void setFile_charset(String file_charset) {
		FileReader.file_charset = file_charset;
	}

	private static String file_charset = null;
	
	public static List<String> getFiles(String dir,String filter) {
		List<String> rst = new ArrayList<String>();
		
		File file = new File(dir);
		// 如果文件夹不存在则创建
		if (!file.exists()) {
			System.out.println("//不存在");
			return null;
		}
		
		//is file,just read it
		if(!file.isDirectory()){
			System.out.println("path is not a directory");
			return null;
		}
		
		//is directory,read all
		if(file.isDirectory()){
			String[] filelist = file.list();
            for (int i = 0; i < filelist.length; i++) {
            	String path = dir + FILE_SEPARATOR + filelist[i]; 
            	if(path.endsWith(filter)){
            		rst.add(path);
            	}
            	
            	System.out.println("read lines " + rst.size());
            }
		}
		
		System.out.println("read finish");
		
		return rst;
	}
	
	public static List<String> getAll(String dir,String filter) {
		List<String> rst = new ArrayList<String>();
		
		File file = new File(dir);
		// 如果文件夹不存在则创建
		if (!file.exists()) {
			System.out.println(dir + " 不存在!");
			return null;
		}
		
		//is file,just read it
		if(!file.isDirectory()){
			return get(dir);
		}
		
		//is directory,read all
		if(file.isDirectory()){
			String[] filelist = file.list();
            for (int i = 0; i < filelist.length; i++) {
            	String path = dir + FILE_SEPARATOR + filelist[i]; 
            	if(path.endsWith(filter)){
            		rst.addAll(get(path));
            	}
            	
            	System.out.println("read lines " + rst.size());
            }
		}
		
		System.out.println("read finish");
		
		return rst;
	}

	public static List<String> get(String sourceFile) {
		InputStream is = null;
		InputStreamReader fr = null;
		BufferedReader br = null;
		try {
			//get charset
			String charset = getFile_charset();
			if (charset == null) {
				charset = codeString(sourceFile);
				if(charset == null){
					System.out.println("unkown charset");
					return null;
				}
			}
			
			is = new FileInputStream(new File(sourceFile));
			fr = new InputStreamReader(is, charset);
			br = new BufferedReader(fr);

			int lc = 0;
			String ch;
			List<String> result = new ArrayList<String>();

			System.out.println("read begin:" + sourceFile);
			while ((ch = br.readLine()) != null) {
				lc++;
				result.add(ch);
			}
			System.out.println("read lines " + lc);

			return result;
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			try {
				if (br != null)
					br.close();
				if (fr != null)
					fr.close();
				if (is != null)
					is.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}

		return null;
	}
	
	public static String codeString(String fileName) throws Exception{
		BufferedInputStream bin = new BufferedInputStream(
				new FileInputStream(fileName));

		String code = null;
		try {
			int p = (bin.read() << 8) + bin.read();
			 
			switch (p) {
			    case 0xefbb:
			        code = "UTF-8";
			        break;
			    case 0xfffe:
			        code = "Unicode";
			        break;
			    case 0xfeff:
			        code = "UTF-16BE";
			        break;
			    default:
			        code = "GBK";
			}
		} catch (Exception e) {
			e.printStackTrace();
		}finally{
			if(bin != null) bin.close();
		}
         
        return code;
    }
}
