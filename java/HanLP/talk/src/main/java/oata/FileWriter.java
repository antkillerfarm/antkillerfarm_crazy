package oata;

import java.io.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

public class FileWriter {
	public static final String FILE_SEPARATOR = System.getProperty("file.separator");  
	
	public static void put(String targetFile,@SuppressWarnings("rawtypes") List data) {
        try {
			OutputStream fos = new FileOutputStream(new File(targetFile));
			OutputStreamWriter osw = new OutputStreamWriter(fos, "UTF-8");   
			BufferedWriter wr = new BufferedWriter(osw);
			try {
				int lc = 0;
				for(Object rchl : data ){
					String rch = null;
					if(rchl != null) rch= rchl.toString();
					lc ++;
					if(rch == null  ||  rch.length() <1)
						continue;
					
					wr.write(rch);
					wr.newLine();
					if(lc % 10000 == 0){
						wr.flush();
						System.out.println("write lines " + lc);
					}
				}
				System.out.println("total write lines " + lc);
			} catch(IOException ioe){
				throw ioe;
			}catch (Exception e) {
				e.printStackTrace();
			}finally{
				if(wr != null)wr.close();
				if(osw != null)osw.close();
				if(fos != null)fos.close();
			}
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	@SuppressWarnings("rawtypes")
	public static void dumpMap(Map m,String file){
    	List<String> dr = new ArrayList<String>();
    	for(Object oentry : m.entrySet()){
    		Entry entry = (Entry)oentry;
    		dr.add(entry.getKey()+","+entry.getValue());
    	}
    	
    	FileWriter.put(file, dr);
    }
}
