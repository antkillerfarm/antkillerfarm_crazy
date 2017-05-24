package com.tj.ai.sentence.models;

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;

import com.tj.ai.base.utils.FileWriter;
import com.tj.ai.sentence.bean.TPhrase;
import com.tj.ai.sentence.bean.TStandard;

public class KnowlegeModel implements Serializable {
	private static final long serialVersionUID = 1L;

	private static KnowlegeModel inst;
	
	private PhraseManager phrases;
	private StandardManager stands;
	
	private KnowlegeModel(){
		phrases = new PhraseManager();
		stands = new StandardManager();
	}
	
	public static KnowlegeModel get() {
		if(inst == null)
			inst = new KnowlegeModel();
		
		return inst;
	}
	
	public void add(String keyword,TPhrase phrase){
		phrases.add(keyword, phrase);
	}
	
	public void add(String keyword,TStandard stand){
		stands.add(keyword, stand);
	}

	public static KnowlegeModel load(String path) {
		//局部变量必须要初始化
		ObjectInputStream oin = null;
        try{
            oin = new ObjectInputStream(new FileInputStream(path));
        } catch (Exception e1) {
            e1.printStackTrace();
        }
        
        KnowlegeModel mts = null;
        try {
        	//由Object对象向下转型为MyTest对象
            mts = (KnowlegeModel) oin.readObject();
        } catch (Exception e) {
            e.printStackTrace();
        }finally{
        	try {
				if(oin != null)
					oin.close();
			} catch (IOException ioe) {
			}
        }
        
        return mts;
	}

	public boolean save(String path) {
		try {
			//输出流保存的文件名为 my.out ；ObjectOutputStream能把Object输出成Byte流
			ObjectOutputStream oos = new ObjectOutputStream(new FileOutputStream(path));
			oos.writeObject(this); 
			oos.flush();  //缓冲流 
			oos.close(); //关闭流
			
			return true;
		} catch (Exception e) {
			e.printStackTrace();
		}
		
		return false;
	}

	public PhraseManager getPhrases() {
		return phrases;
	}

	public void setPhrases(PhraseManager phrases) {
		this.phrases = phrases;
	}

	public StandardManager getStands() {
		return stands;
	}

	public void setStands(StandardManager stands) {
		this.stands = stands;
	}

	public void dump(String dir) {
		FileWriter.put(dir+"dump.txt", this.stands.explain());
	}
}
