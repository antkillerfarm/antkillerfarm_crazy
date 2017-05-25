package com.ansj.vec.LDA;

import com.tj.ai.base.bean.Vocabulary;

import java.io.*;
import java.util.*;
import java.util.Map.Entry;

public class LDAService implements Serializable {
	private static final long serialVersionUID = 1L;

	private static final LDAService inst = new LDAService();
	
	private double[][] phi;
	private double[][] theta;
	
	private LDAService(){
	}
	
	public static LDAService getInst(){
		return inst;
	}

	public void train(String path){
		try {
			// 1. �Ӵ�����������
			Corpus corpus = Corpus.load("data/mini");
			// 2. ���� LDA ������
			LdaGibbsSampler ldaGibbsSampler = new LdaGibbsSampler(corpus.getDocument(), corpus.getVocabularySize());
			// 3. ѵ����Ŀ��10������
			ldaGibbsSampler.gibbs(5);
			// 4. phi ������Ψһ���õĶ������� LdaUtil ��չʾ���յĽ��
			phi = ldaGibbsSampler.getPhi();
			theta = ldaGibbsSampler.getTheta();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	public void train(List<List<Integer>> data,int size){
		try {
			LdaGibbsSampler ldaGibbsSampler = new LdaGibbsSampler(getDocument(data), Vocabulary.getInst().size());
			ldaGibbsSampler.gibbs(size);
			phi = ldaGibbsSampler.getPhi();
			theta = ldaGibbsSampler.getTheta();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	public void trainEx(List<List<String>> data,int size){
		try {
			Corpus corpus = Corpus.load(data);
			LdaGibbsSampler ldaGibbsSampler = new LdaGibbsSampler(corpus.getDocument(), Vocabulary.getInst().size());
			ldaGibbsSampler.gibbs(size);
			phi = ldaGibbsSampler.getPhi();
			theta = ldaGibbsSampler.getTheta();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	private int[][] getDocument(List<List<Integer>> data) {
		List<int[]> doca = new ArrayList<int[]>();
		for(int i=0;i<data.size();i++){
			List<Integer> dat = data.get(i);
			int[] datai = new int[dat.size()];
			for(int j=0;j<dat.size();j++){
				datai[j] = dat.get(j);
			}
			doca.add(datai);
		}
		
		return doca.toArray(new int[0][]);
		/*
		int[][] rst = new int[data.size()][data.get(0).size()];
		for(int i=0;i<data.size();i++){
			List<Integer> dat = data.get(i);
			for(int j=0;j<dat.size();j++){
				rst[i][j] = dat.get(j);
			}	
		}
		return rst;*/
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
            LDAService ins = (LDAService) oin.readObject();
            this.phi = ins.phi;
            this.theta = ins.theta;
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
	
	public static void init(String path){
		if(path != null){
			inst.load(path);
		}
	}
	
	public Map<String, Double>[] translate(int limit){
		return LdaUtil.translate(phi, Vocabulary.getInst(), limit);
	}
	
	public TreeMap<Float, Integer> evaluateEx(List<Integer> data){
		return evaluateEx((Integer[])data.toArray(new Integer[data.size()]));
	}
	
	public TreeMap<Float, Integer> evaluateEx(Integer[] data){
		//get index of string
		List<Integer> idxs = new ArrayList<Integer>();
		Vocabulary vb = Vocabulary.getInst();
		for(Integer idx : data){
			if(idx < vb.size())
				idxs.add(idx);
		}
		
		int[] rr = new int[this.phi.length];
		//get ratio of each words
		for(Integer ea : data){
			double v = -99999;
			int ri = -1;
			for(int i=0;i<this.phi.length;i++){
				if(v < this.phi[i][ea]){
					v = this.phi[i][ea];
					ri = i;
				}
			}
			rr[ri] = rr[ri] + 1;
		}

		TreeMap<Float, Integer> rankMap = new TreeMap<Float, Integer>(Collections.reverseOrder());
		if(idxs.size() == 0)
			return rankMap;
		
        for (int i = 0; i < rr.length; i++) {
            rankMap.put((float)rr[i]/idxs.size(),i);
        }
        
        return rankMap;
	}
	
	public Map<Float,Integer> evaluate(List<String> data){
		String[] dataa = new String[data.size()];
		for(int i=0;i<data.size();i++){
			dataa[i] = data.get(i);
		}
		
		//Map<String, Double>[] topicMap = LdaUtil.translate(phi, Vocabulary.getInst(), 10);
		//LdaUtil.explain(topicMap);
		
		return evaluate(dataa);
	}
	
	public Map<Float,Integer> evaluate(String[] data){
		//get index of string
		List<Integer> idxs = new ArrayList<Integer>();
		Vocabulary vb = Vocabulary.getInst();
		for(String str : data){
			Integer idx = vb.getId(str, false);
			if(idx != null)
				idxs.add(idx);
		}
		
		int[] rr = new int[this.phi.length];
		//get ratio of each words
		for(Integer ea : idxs){
			double v = -99999;
			int ri = -1;
			for(int i=0;i<this.phi.length;i++){
				if(v < this.phi[i][ea]){
					v = this.phi[i][ea];
					ri = i;
				}
			}
			rr[ri] = rr[ri] + 1;
		}

		TreeMap<Float, Integer> rankMap = new TreeMap<Float, Integer>(Collections.reverseOrder());
		if(idxs.size() == 0)
			return rankMap;
		
        for (int i = 0; i < rr.length; i++) {
            rankMap.put((float)rr[i]/idxs.size(),i);
        }
        
        return rankMap;
	}
	
	public static void main(String[] args){
		try {
			// 1. �Ӵ�����������
			Corpus corpus = Corpus.loadFile("/home/tj/my/work/ai/tj-ai/tj-ai-lda/data/mini/2.txt");
			// 2. ���� LDA ������
			LdaGibbsSampler ldaGibbsSampler = new LdaGibbsSampler(corpus.getDocument(), corpus.getVocabularySize());
			// 3. ѵ����Ŀ��10������
			ldaGibbsSampler.gibbs(5);
			// 4. phi ������Ψһ���õĶ������� LdaUtil ��չʾ���յĽ��
			double[][] phi = ldaGibbsSampler.getPhi();
			
			Map<String, Double>[] topicMap = LdaUtil.translate(phi, corpus.getVocabulary(), 10);
			LdaUtil.explain(topicMap);
			
			Vocabulary.getInst().save("/home/tj/my/work/ai/tj-ai/tj-ai-lda/data/vocabulary.bin");
			
			LDAService lda = LDAService.getInst();
			lda.setPHI(phi);
			//Vocabulary.getInst().load("/home/tj/my/work/ai/tj-ai/tj-ai-lda/data/vocabulary.bin");
			//lda.load("c");
			//lda.load("/home/tj/my/work/ai/tj-ai/tj-ai-lda/data/pha.txt");
			//evaluate
			String ds = "老板 话费 没 马 查询 偶尔 会 有 情况 建议 拨打 电话 电信 转 人工 查询 缴费 记录 带来 非常 收到 好评 算 打 问问 没 人 接 打 人工 还有 办法 号码 订单 已 提交 核实 如 有 结果 会 时间 回复 请 等待 充 没 充 去 才 换 交易 话费 没 到 醉 已 提交 核实 时候 有 结果 核实 话 最 是 小时 有 结果 话 小时 就 会 有 结果 没有 记录 订单 有 延迟 情况 最 延迟 小时 请 用户 关注 小时 情况 带来 真 订单 是 可 延迟 到 最 延迟 小时 到 建议 随时 关注 进 情况 退款 如 返回 不 会 退回 醉 还 没 到 查询 估计 到 不 看 办 核实 已经 都 是 发送 有时候 电信 业务 会 有 停机 欠费 发送 失败 情况 是 建议 拨打 电话 转 人工 查询 记录 不 是 余额 电信 都 打 问 说 没有 记录 存 查询 没有 记录 提供 手机 密码 进一步 核实 密码 是 拨打 电话 查询 电信 记录 可 营业厅 查询 拨 拨打 电话 查询 提供 手机 密码 进一步 核实 问 核实 已经 不 知道 密码 可以 拨打 热线 人工 叫 帮 置 密码 电信 就 打 勒 去 去 打 都 没 人 接 才 加 公众 说 都 没有 记录 要 搞 查询 密码 提供 密码 核实 是 帮 父母 充 手机 没 可以 到 时候 问道 密码 联系 会 核实 是 原因 有 核实 是 已经 到 时候 提供 密码 核实 如 到 时候 最终 核实 失败 到时 看看 是 要 退款 补充 还 是 换 冲 密码 核实 有 会 告诉 延迟 小时 没有 会 退款 退款 过程 是 退款";
			String[] es = ds.split(" ");
			Map<Float,Integer> rm = lda.evaluate(es);
			explain(rm);
			
			ds = "充 钱 到 没 申请 退款 请 系统 显示 可 是 没 发 只 有 钱 可以 提供 密码 再 花 也 不 可能 花 提供 密码 后天 核实 可以 拨打 人工 置 冲 完 都 没 收到 充 钱 到 没收 到 是 回 事 核实 已经 都 是 发送 有时候 电信 业务 会 有 停机 欠费 发送 失败 情况 是 建议 拨打 电话 转 人工 查询 记录 不 是 余额 电信 可 是 都 没 收到 可 是 都 没 收到 是 发送 有时候 会 出现 情况 建议 拨打 电话 转 人工 查询 记录 不 是 余额 电信 查 只 有 钱 再 打电话 也 不 可能 花 钱 不 是 查询 余额 是 查询 记录 有 欠费 停机 情况 导致 手机 还 是 不能 拨打 电话 情况 查询 余额 有 误 建议 免费 电话 查询 记录 电信 登录 官方 网站 查询 缴费 记录 说 没有 未 提供 手机 密码 进一步 核实 密码 是 拨打 电话 查询 电信 记录 可 营业厅 查询 拨 拨打 电话 查询 有 可能 只 有 钱 都 是 扣 月 租 可以 拨打 热线 人工 查询 密码 叫 帮 置 密码 电信 就 打 登录 密码 就是 来 查询 话费 记录 情况 月 租 只 有 钱 请 后台 核实 可能 需要 时间 请 尽快 已经 提 交给 后台 请 请问 还有 需要 协助 查 到 充 钱 话费 没 请 马 查询 已经 已经 提 交给 后台 知道 显示 也 是 交易 话费 一直 没 到 是 回 事 充 完 钱 都 会 有 提醒 也 一直 没有 收到 已经 帮 也 不 是 说 讹诈 还 是 就是 想 搞 钱 去向 毕竟 不 是 钱 尽快 解决 请 放心 会 尽快 帮 处理 带来 真的 很 请问 还有 需要 协助 订单 核实 没有 马 补充 可以 带来 非常 感谢 商城 支持 期待 宝贝 满意 请 记得 好评 祝 订单 核实 话费 不 已经 补充 请 核实 缴费 记录 稍后 查询 到 缴费 记录 帮忙 取消 退款 申请";
			es = ds.split(" ");
			rm = lda.evaluate(es);
			explain(rm);
			
			ds = "发货 冲 流量 时候 到 流量 就 可以 最 不 会 超过 小时 建议 随时 关注 情况 带来";
			es = ds.split(" ");
			rm = lda.evaluate(es);
			explain(rm);
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	public static void explain(Map<Float,Integer> result){
        for (Entry<Float,Integer> entry : result.entrySet()){
            System.out.printf("topic %f : %d", entry.getKey(),entry.getValue());
            System.out.println();
        }
        
        System.out.println();
	}
	
	public void setPHI(double[][] data){
		this.phi = data;
	}
	public double[][] getPHI(){
		return this.phi;
	}
	public void setTheta(double[][] data){
		this.theta = data;
	}
	public double[][] getTheta(){
		return this.theta;
	}
}
