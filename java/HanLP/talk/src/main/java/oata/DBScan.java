package oata;

import org.apache.commons.lang3.StringUtils;

import java.util.*;

public class DBScan {
    double Eps=0.8;   //区域半径
    int MinPts=4;   //密度
    class DataObject {
        public boolean isVisited() {
            return visited;
        }

        public void setVisited(boolean visited) {
            this.visited = visited;
        }

        boolean visited = false;

        public int getCid() {
            return cid;
        }

        public void setCid(int cid) {
            this.cid = cid;
        }

        int cid = 0;

        public String getValue() {
            return value;
        }

        public void setValue(String value) {
            this.value = value;
        }

        String value = null;
    }

    //由于自己到自己的距离是0,所以自己也是自己的neighbor
    public Vector<DataObject> getNeighbors(DataObject p, ArrayList<DataObject> objects){
        Vector<DataObject> neighbors=new Vector<DataObject>();
        Iterator<DataObject> iter=objects.iterator();
        while(iter.hasNext()){
            DataObject q=iter.next();
            String arr1=p.getValue();
            String arr2=q.getValue();

            if(calJaccardDist(arr1,arr2)<=Eps){      //使用编辑距离
//          if(Global.calEuraDist(arr1, arr2, len)<=Eps){    //使用欧氏距离
//          if(Global.calCityBlockDist(arr1, arr2, len)<=Eps){   //使用街区距离
//          if(Global.calSinDist(arr1, arr2, len)<=Eps){ //使用向量夹角的正弦
                neighbors.add(q);
            }
        }
        return neighbors;
    }

    public int dbscan(ArrayList<DataObject> objects){
        int clusterID=0;
        boolean AllVisited=false;
        while(!AllVisited){
            Iterator<DataObject> iter=objects.iterator();
            while(iter.hasNext()){
                DataObject p=iter.next();
                if(p.isVisited())
                    continue;
                AllVisited=false;
                p.setVisited(true);     //设为visited后就已经确定了它是核心点还是边界点
                Vector<DataObject> neighbors=getNeighbors(p,objects);
                if(neighbors.size()<MinPts){
                    if(p.getCid()<=0)
                        p.setCid(-1);       //cid初始为0,表示未分类；分类后设置为一个正数；设置为-1表示噪声。
                }else{
                    if(p.getCid()<=0){
                        clusterID++;
                        expandCluster(p,neighbors,clusterID,objects);
                    }else{
                        int iid=p.getCid();
                        expandCluster(p,neighbors,iid,objects);
                    }
                }
                AllVisited=true;
            }
        }
        return clusterID;
    }

    private void expandCluster(DataObject p, Vector<DataObject> neighbors,
                               int clusterID,ArrayList<DataObject> objects) {
        p.setCid(clusterID);
        Iterator<DataObject> iter=neighbors.iterator();
        while(iter.hasNext()){
            DataObject q=iter.next();
            if(!q.isVisited()){
                q.setVisited(true);
                Vector<DataObject> qneighbors=getNeighbors(q,objects);
                if(qneighbors.size()>=MinPts){
                    Iterator<DataObject> it=qneighbors.iterator();
                    while(it.hasNext()){
                        DataObject no=it.next();
                        if(no.getCid()<=0)
                            no.setCid(clusterID);
                    }
                }
            }
            if(q.getCid()<=0){       //q不是任何簇的成员
                q.setCid(clusterID);
            }
        }
    }

    /*public static void main(String[] args){

        DBScan ds=new DBScan();
        int clunum=ds.dbscan(datasource.objects);
        datasource.printResult(datasource.objects,clunum);
    }*/
    public ArrayList<DataObject> formDataObject(List<String> str_list) {
        ArrayList<DataObject> dataObjects = new ArrayList<>();
        for (String str : str_list) {
            DataObject object = new DataObject();
            object.setValue(str);
            dataObjects.add(object);
        }
        return dataObjects;
    }
    public static double calJaccardDist(String s1, String s2) {
        String[] s1s = StringUtils.split(s1, " ");
        String[] s2s = StringUtils.split(s2, " ");
        List<String> s1_list = Arrays.asList(s1s);
        List<String> s2_list = Arrays.asList(s2s);
        Set<String> union = new HashSet<String>();
        union.addAll(s1_list);
        union.addAll(s2_list);

        double x = s1s.length;
        double y = s2s.length;
        double z = union.size();
        return 1.0 - ((x + y - z) / z);
    }
    public static double calJaccardDist2(String s1, String s2) {
        String[] s1s = StringUtils.split(s1, " ");
        String[] s2s = StringUtils.split(s2, " ");
        List<String> s1_list = Arrays.asList(s1s);
        List<String> s2_list = Arrays.asList(s2s);
        Set<String> union = new HashSet<String>();
        union.addAll(s1_list);
        union.addAll(s2_list);

        double x = s1s.length;
        double y = s2s.length;
        double min = Math.min(x,y);
        double z = union.size();
        return 1.0 - ((x + y - z) / min);
    }
}
