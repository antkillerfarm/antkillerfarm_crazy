package oata;

import java.util.Collections;
import java.io.IOException;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;

import com.fasterxml.jackson.databind.JsonNode;
import org.apache.http.HttpHost;
import org.apache.http.entity.ContentType;
import org.apache.http.nio.entity.NStringEntity;
import org.elasticsearch.client.Response;
import org.elasticsearch.client.ResponseException;
import org.elasticsearch.client.RestClient;
import org.apache.commons.io.IOUtils;
import com.fasterxml.jackson.databind.ObjectMapper;

public class HelloWorld {
    public static RestClient client;
    //private static String host = "localhost";
    private static String host = "192.168.4.251";
    private static String index_name = "knowledge_lib2";

    public static void main(String[] args) {
        System.out.println("Hello World");
        long startTime=System.currentTimeMillis();
        client = RestClient.builder(new HttpHost(host, 9200, "http")).build();
        long endTime=System.currentTimeMillis();
        System.out.println("Time 1"+(endTime-startTime)+"ms");

        try {
            startTime = endTime;
            //test1();
            //test2();
            //test3();
            test4();
            //test5();
            endTime=System.currentTimeMillis();
            System.out.println("Time 2"+(endTime-startTime)+"ms");
            startTime = endTime;
            if (client != null) {
                client.close();
            }
            endTime=System.currentTimeMillis();
            System.out.println("Time 3"+(endTime-startTime)+"ms");
        } catch(Exception e)
        {
            e.printStackTrace();
        }
    }

    public static void test1() throws IOException {
        String resource = "/_cat/health";
        String body = "";
        Response response = client.performRequest("GET", resource, Collections.singletonMap("pretty", "true"),
                new NStringEntity(body, ContentType.APPLICATION_JSON));
        System.out.println(IOUtils.toString(response.getEntity().getContent(), "UTF-8"));
    }

    public static void test2() throws IOException {
        String resource = index_name + "/_search";
        String body = "{\n" +
                "  \"query\": {\n" +
                "    \"match\": { \"ShortCutID\": \"购物车\" }\n" +
                "  }\n" +
                "}";
        Response response = client.performRequest("GET", resource, Collections.singletonMap("pretty", "true"),
                new NStringEntity(body, ContentType.APPLICATION_JSON));
        System.out.println(IOUtils.toString(response.getEntity().getContent(), "UTF-8"));
    }

    public static void test3() throws IOException {
        String resource = index_name + "/_search";
        String body = "{\n" +
                "  \"query\": {\n" +
                "    \"match\": { \"ShortCutID\": \"购物车\" }\n" +
                "  }\n" +
                "}";
        Response response = client.performRequest("GET", resource, Collections.singletonMap("pretty", "true"),
                new NStringEntity(body, ContentType.APPLICATION_JSON));
        ObjectMapper mapper = new ObjectMapper();
        Map<String, Map<String, Object>> maps =
                mapper.readValue(IOUtils.toString(response.getEntity().getContent(), "UTF-8"), Map.class);
        System.out.println(maps.size());
        Set<String> key = maps.keySet();
        Iterator<String> iter = key.iterator();
        while (iter.hasNext()) {
            String field = iter.next();
            System.out.println(field + ":" + maps.get(field));
        }
    }

    public static void test4() throws IOException {
        String resource = "/" + index_name + "/_search";
        String body = "{\n" +
                "  \"query\": {\n" +
                "    \"match\": { \"ShortCutID\": \"购物车\" }\n" +
                //"    \"match\": { \"industry_id\": \"购物车\" }\n" +
                "  }\n" +
                "}";

        Response response = null;
        int status = 0;
        try {
            response = client.performRequest("GET", resource, Collections.singletonMap("pretty", "true"),
                    new NStringEntity(body, ContentType.APPLICATION_JSON));
        } catch (ResponseException e) {
            status = 1;
            System.out.println(e.getResponse().getStatusLine().toString());
        }
        System.out.println("status:" + status);
        if (status == 0) {
            ObjectMapper mapper = new ObjectMapper();
            JsonNode rootNode = mapper.readTree(IOUtils.toString(response.getEntity().getContent(), "UTF-8"));
            int took = rootNode.path("took").asInt();
            System.out.println("took:" + took);
            JsonNode node = rootNode.path("hits");
            int total = node.path("total").asInt();
            System.out.println("hits:" + total);
            node = node.path("hits");
            for (int i = 0; i < node.size(); i++) {
                JsonNode node2 = node.get(i);
                node2 = node2.path("_source");
                String s = node2.path("ShortCutID").asText();
                System.out.println(i + ":" + s);
            }
        }
    }


    public static void test5() throws IOException {
        String resource = index_name +"/question/2";
        String question = null;
        if (question == null)
        {
            question = "null";
        }
        else
        {
            question = "\"" + question + "\"";
        }
        String body = String.format("{\n" +
                "  \"industry_id\" : 123,\n" +
                "  \"merchant_id\" : 456,\n" +
                "  \"shop_id\" : 789,\n" +
                "  \"customer_id\" : 1644,\n" +
                "  \"short_question\" : \"充值中\",\n" +
                "  \"question\": %s\n" +
                "}",question);
        Response response = client.performRequest("PUT", resource, Collections.singletonMap("pretty", "true"),
                new NStringEntity(body, ContentType.APPLICATION_JSON));

        int status = getStatus(response);
        //ObjectMapper mapper = new ObjectMapper();
        //JsonNode rootNode = mapper.readTree(IOUtils.toString(response.getEntity().getContent(), "UTF-8"));
        //int status = rootNode.path("status").asInt();
        System.out.println("status:" + status);
    }

    private static int getStatus(Response response) throws IOException {
        ObjectMapper mapper = new ObjectMapper();
        JsonNode rootNode = mapper.readTree(IOUtils.toString(response.getEntity().getContent(), "UTF-8"));
        int status = rootNode.path("status").asInt();
        //System.out.println("status:" + status);
        return status;
    }
}
