package com.ansj.vec.LDA;

import java.io.*;
import java.util.Map;
import java.util.TreeMap;

/**
 * a word set
 *
 * @author hankcs
 */
public class Vocabulary implements Serializable {
	private static final long serialVersionUID = 1L;
	
	Map<String, Integer> word2idMap;
	String[] id2wordMap;
	private boolean needSave = false;
	private String path = "./";
	
	private static Vocabulary inst = new Vocabulary();
	
	public static Vocabulary getInst(){
		return inst;
	}
	
	private Vocabulary() {
		word2idMap = new TreeMap<String, Integer>();
		id2wordMap = new String[1024];
	}

	public Integer getId(String word) {
		return getId(word, false);
	}

	public String getWord(int id) {
		return id2wordMap[id];
	}

	public Integer getId(String word, boolean create) {
		Integer id = word2idMap.get(word);
		if (!create)
			return id;
		if (id == null) {
			id = word2idMap.size();
		}
		word2idMap.put(word, id);
		if (id2wordMap.length - 1 < id) {
			resize(word2idMap.size() * 2);
		}
		id2wordMap[id] = word;

		setNeedSave(true);
		
		return id;
	}

	private void resize(int n) {
		String[] nArray = new String[n];
		System.arraycopy(id2wordMap, 0, nArray, 0, id2wordMap.length);
		id2wordMap = nArray;
	}

	@SuppressWarnings("unused")
	private void loseWeight() {
		if (size() == id2wordMap.length)
			return;
		resize(word2idMap.size());
	}

	public int size() {
		return word2idMap.size();
	}

	@Override
	public String toString() {
		final StringBuilder sb = new StringBuilder();
		for (int i = 0; i < id2wordMap.length; i++) {
			if (id2wordMap[i] == null)
				break;
			sb.append(i).append("=").append(id2wordMap[i]).append("\n");
		}
		return sb.toString();
	}
	

    public boolean save(String path){
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
	
	public void load(String path){
		//局部变量必须要初始化
		ObjectInputStream oin = null;
        try{
            oin = new ObjectInputStream(new FileInputStream(path));
        } catch (Exception e1) {
            e1.printStackTrace();
        }
        
        try {
        	//由Object对象向下转型为MyTest对象
        	Vocabulary ins = (Vocabulary) oin.readObject();
            this.id2wordMap = ins.id2wordMap;
            this.word2idMap = ins.word2idMap;
        } catch (Exception e) {
            e.printStackTrace();
        }finally{
        	try {
				if(oin != null)
					oin.close();
			} catch (IOException ioe) {
			}
        }
	}

	public static void init(String path) {
		if(path != null){
			getInst().load(path);
			getInst().setPath(path);
		}
	}

	public void close(){
		if(this.needSave)
			save(path);
	}
	
	/**
	 * @return the needSave
	 */
	public boolean isNeedSave() {
		return needSave;
	}

	/**
	 * @param needSave the needSave to set
	 */
	public void setNeedSave(boolean needSave) {
		this.needSave = needSave;
	}

	/**
	 * @return the path
	 */
	public String getPath() {
		return path;
	}

	/**
	 * @param path the path to set
	 */
	public void setPath(String path) {
		this.path = path;
	}
}
