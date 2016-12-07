/*
 * Copyright 2013 Himanshu Bhardwaj
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
*/
package com.himanshu.poc.jaxws.document_client;

import java.net.MalformedURLException;
import java.net.URL;
import java.util.logging.Logger;

import javax.xml.namespace.QName;

import gen.com.himanshu.poc.jaxws.stubs.document_type.HelloWorldServiceImplService;

public class HelloWorldServiceImplServiceExtension extends HelloWorldServiceImplService {

	private final static URL HELLOWORLDSERVICEIMPLSERVICE_WSDL_LOCATION;
    private final static Logger logger = Logger.getLogger(HelloWorldServiceImplServiceExtension.class.getName());

	static {
        URL url = null;
        try {
            URL baseUrl;
            baseUrl = gen.com.himanshu.poc.jaxws.stubs.document_type.HelloWorldServiceImplService.class.getResource(".");
            url = new URL(baseUrl, "http://localhost:9999/ws/hellodocument");	//Configurable here, read from properties or something
        } catch (MalformedURLException e) {
            logger.warning("Failed to create URL for the wsdl Location: 'http://localhost:9999/ws/hellodocument', retrying as a local file");
            logger.warning(e.getMessage());
        }
        HELLOWORLDSERVICEIMPLSERVICE_WSDL_LOCATION = url;
    }

	public HelloWorldServiceImplServiceExtension() {
        super(HELLOWORLDSERVICEIMPLSERVICE_WSDL_LOCATION, new QName("http://document_type.service.jaxws.poc.himanshu.com/", "HelloWorldServiceImplService"));
	}

}
