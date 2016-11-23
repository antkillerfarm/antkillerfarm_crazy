package oata;

import java.util.ArrayList;
import java.util.List;
import java.io.IOException;

import org.codehaus.jackson.annotate.JsonAutoDetect.Visibility;
import org.codehaus.jackson.annotate.JsonMethod;
import org.codehaus.jackson.map.ObjectMapper;
import org.codehaus.jackson.annotate.JsonAutoDetect;

public class HelloWorld {
    public static class A{
        String name;
        int age;
        List<String> test = new ArrayList<String>();
    }
    public static void main(String[] args) throws IOException {
	ObjectMapper mapper = new ObjectMapper();
        mapper = new ObjectMapper();
        mapper.setVisibility(JsonMethod.FIELD, JsonAutoDetect.Visibility.ANY);
	//        mapper.getSerializationConfig().setSerializationInclusion(inclusion);

        mapper.getDeserializationConfig().set(
					      org.codehaus.jackson.map.DeserializationConfig.Feature.FAIL_ON_UNKNOWN_PROPERTIES, false);
        A a = new A();
        a.name = "zhangsan";
        a.age = 10;
        a.test.add("fuck");
        a.test.add("god");
        A b = new A();
        b.name = "lisi" ;
        b.age = 12;
        b.test.add("fuck");
        b.test.add("god");
        A[] list = {a,b};
        System.out.println(mapper.writeValueAsString(a));
        String result = mapper.writeValueAsString(list);

	//        List<Object> result = mapper.readValue("",List.class);
        System.out.println(result);
	System.out.println("Hello");
    }
}
