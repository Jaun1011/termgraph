
# Utils

```java
public class Converter {

    public static byte[] serialize(Transfer obj) throws IOException {
        ByteArrayOutputStream out = new ByteArrayOutputStream();
        ObjectOutputStream os = new ObjectOutputStream(out);
        os.writeObject(obj);
        return out.toByteArray();
    }

    public static Object deserialize(byte[] data) throws IOException, ClassNotFoundException {
        ByteArrayInputStream in = new ByteArrayInputStream(data);
        ObjectInputStream is = new ObjectInputStream(in);
        return is.readObject();
    }
}

```


# TCP
## Server

```java
// TcpSocket tcpSocket = new TcpSocket(6900);
// tcpSocket.start()
public class TcpSocket extends Thread {
    int port;

    Bank bank;

    Socket socket;
    ObjectInputStream in;
    ObjectOutputStream out;


    public TcpSocket(int port) {
        this.port = port;
        this.bank = new Bank();
    }

    public void run() {
        System.out.println("TcpSocket on port: " + port);

        try {
            ServerSocket server = new ServerSocket(port);

            socket = server.accept();
            in = new ObjectInputStream(socket.getInputStream());
            out = new ObjectOutputStream(socket.getOutputStream());

            while (true) {

                Transfer transfer = (Transfer) in.readObject();

                System.out.println("IN\t" + transfer.toString());

                Transfer result = this.bank.transferCall(transfer);

                System.out.println("OUT\t" + result.toString());
                out.writeObject(result);
                out.flush();
            }
        } catch (IOException | ClassNotFoundException e) {
            e.printStackTrace();
        }
    }
}

```
## Client
```java
public class TcpDriver implements Driver {
    BankProxy bankProxy;

    Socket socket;

    ObjectOutputStream out;
    ObjectInputStream in;

    @Override
    public void connect(String[] args) throws IOException {
        String host = args[0];
        String port = args[1];

        socket = new Socket(host, Integer.parseInt(port), null, 0);
        System.out.println("connect called with arguments " + Arrays.deepToString(args));

        out = new ObjectOutputStream(socket.getOutputStream());
        in = new ObjectInputStream(socket.getInputStream());

        bankProxy = new BankProxy(this);
    }

    @Override
    public void disconnect() throws IOException {
        socket.close();
    }

    @Override
    public Transfer send(Transfer transfer) {
        try {

            System.out.println("OUT\t" + transfer.toString());

            out.writeObject(transfer);
            out.flush();

            Transfer result = (Transfer) in.readObject();
            System.out.println("IN\t" + result.toString());

            return result;

        } catch (IOException | ClassNotFoundException e) {
            e.printStackTrace();
        }
        return null;
    }
}

```

# UDP
## Server

```java
public class UdpSocket extends Thread {
    int port;
    Bank bank;

    public static final int BUFFER_SIZE = 10000;
    Map<InetAddress, Transfer> requests;

    public UdpSocket(int port) {
        this.port = port;
        this.bank = new Bank();
        requests = new HashMap<>();
    }

    private void send(DatagramSocket socket, DatagramPacket packet, Transfer transfer) throws IOException {
        DatagramPacket packageResponse = new DatagramPacket(new byte[BUFFER_SIZE], BUFFER_SIZE);
        packageResponse.setAddress(packet.getAddress());
        packageResponse.setPort(packet.getPort());

        packageResponse.setData(Converter.serialize(transfer));

        System.out.println("OUT\t" + transfer.toString());
        socket.send(packet);
    }

    public void run() {
        try (DatagramSocket socket = new DatagramSocket(port)) {
            System.out.println("UdpSocket on port: " + port);
            DatagramPacket packet = new DatagramPacket(new byte[BUFFER_SIZE], BUFFER_SIZE);

            while (true) {
                socket.receive(packet);

                System.out.println("IN\tconnection inet " + packet.getAddress() + " port " + packet.getPort());

                Transfer transfer = Converter.deserialize(packet.getData());
                System.out.println("IN\t" + transfer.toString());

                int lastTransactionId = -1;
                if (requests.get(packet.getAddress()) != null) {
                    lastTransactionId = requests.get(packet.getAddress()).getTransactionId();
                }

                if (lastTransactionId!= transfer.getTransactionId()) {
                    Transfer result = bank.transferCall(transfer);
                    requests.put(packet.getAddress(), transfer);
                    send(socket, packet, result);
                }
            }
        } catch (ClassNotFoundException | IOException e) {
            e.printStackTrace();
        }
    }
}


```


## Client

```java
public class UdpDriver implements Driver {
    BankProxy bankProxy;

    DatagramSocket socket;

    ObjectOutputStream out;
    ObjectInputStream in;

    @Override
    public void connect(String[] args) throws IOException {
        String host = args[0];
        int port = Integer.parseInt(args[1]);

        socket = new DatagramSocket();
        socket.connect(new InetSocketAddress(host, port));
        socket.setSoTimeout(2000);

        bankProxy = new BankProxy(this);
    }

    @Override
    public void disconnect() throws IOException {
        socket.close();
    }


    int id = 0;

    @Override
    public Transfer send(Transfer transfer) {
        try {
            id++;
            System.out.println("transactionId" + id);
            transfer.setTransactionId(id);
            System.out.println("OUT\t" + transfer.toString());

            byte[] buffer = UdpSocket.toByteArray(transfer);
            DatagramPacket datagramPacket = new DatagramPacket(buffer, buffer.length);

            System.out.println("size " + buffer.length);
            DatagramPacket result = new DatagramPacket(new byte[UdpSocket.BUFFER_SIZE], UdpSocket.BUFFER_SIZE);


            for (int i = 0; i < 5; i++) {

                socket.send(datagramPacket);
                socket.receive(result);

                Transfer response = UdpSocket.getPackageData(result.getData());

                if (response.getTransactionId() == transfer.getTransactionId()){
                    return response;
                }
                Thread.sleep(500);
            }

        } catch (IOException | ClassNotFoundException e) {
            e.printStackTrace();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        return null;
    }
}

```


# Servlets

|HTTP method| RFC    | Request has Body| Response has Body | Safe |	Idempotent  | Cacheable    |
|-----------|--------|-----------------|-------------------|------|-------------|--------------|
|GET 	    |RFC 7231| 	Optional       | Yes               | Yes  | Yes         | Yes          |
|HEAD 	    |RFC 7231| 	Optional       | No                | Yes  |	Yes         | Yes          |
|POST 	    |RFC 7231| 	Yes            | Yes               | No   | 	No| 	Yes|
|PUT 	    |RFC 7231| 	Yes| 	Yes| 	No| 	Yes| 	No|
|DELETE 	|RFC 7231| 	Optional| 	Yes| 	No| 	Yes| 	No|
|CONNECT 	|RFC 7231| 	Optional| 	Yes| 	No| 	No| 	No|
|OPTIONS 	|RFC 7231| 	Optional| 	Yes| 	Yes| 	Yes| 	No|
|TRACE 	    |RFC 7231| 	No| 	Yes| 	Yes| 	Yes| 	No|
|PATCH 	    |RFC 5789| 	Yes| 	Yes| 	No| 	No| 	No| 


## JSON RPC
```json
{ 
	"jsonrpc":"2.0", 
	"id":"939670835", 
	"result":null,

	"error": {
		"code":-32001, 
		"message":"account not active", 
		"data":{
			"exceptionTypeName":"bank.InactiveException",
			"message":"account not active"
		}
	}
}
```

```java
String path = "http://localhost:8080/json-rpc";
JsonRpcHttpClient client = new JsonRpcHttpClient(new URL(path));
Service service = ProxyUtil.createClientProxy(
getClass().getClassLoader(),
Service.class,
client);
```


## Server

```java
public class HttpDemoServer {

	public static void main(String[] args) throws IOException {
		HttpServer server = HttpServer.create(new InetSocketAddress(8080), 0);
		server.createContext("/date", new DateHandler());
		server.createContext("/bank", new BankHandler()).getFilters().add(new ParameterParser());
		server.createContext("/echo", new EchoHandler()).getFilters().add(new ParameterParser());
		
		server.start();
		System.out.println("server started on " + server.getAddress());
	}

	static class DateHandler implements HttpHandler {
		@Override
		public void handle(HttpExchange exchange) throws IOException {
			exchange.getResponseHeaders().add("Content-type", "text/html");
			String response = "<b>" + new Date() + "</b> for " + exchange.getRequestURI();
			exchange.sendResponseHeaders(200, response.length());
			// exchange.sendResponseHeaders(200, 0);
			try(OutputStream os = exchange.getResponseBody()) {
				os.write(response.getBytes());
			}
		}
	}
	
	static class EchoHandler implements HttpHandler {
		@SuppressWarnings("unchecked")
		@Override
		public void handle(HttpExchange httpExchange) throws IOException {
			ByteArrayOutputStream bos = new ByteArrayOutputStream();
			Writer w = new BufferedWriter(new OutputStreamWriter(bos));

			w.append(httpExchange.getRequestMethod() + " ");
			w.append(httpExchange.getRequestURI() + "\n\n");

			for (Entry<String, List<String>> e : httpExchange.getRequestHeaders().entrySet()) {
				w.append(e.getKey() + ": ");
				Iterator<String> it = e.getValue().iterator();
				while (it.hasNext()) {
					w.append(it.next());
					if (it.hasNext()) w.append(", ");
				}
				w.append("\n");
			}
			w.append("\n");
			
			Map<String, Object> params = (Map<String, Object>) httpExchange.getAttribute("parameters");
			if (params != null) {
				for (Entry<String, Object> e : params.entrySet()) {
					w.append(e.getKey() + " = " + e.getValue() + "\n");
				}
			}
			
			w.flush();
			w.close();
			
			byte[] buf = bos.toByteArray();
			httpExchange.getResponseHeaders().add("Content-type", "text/plain");
			httpExchange.sendResponseHeaders(200, buf.length);
			OutputStream os = httpExchange.getResponseBody();
			os.write(buf);
			os.close();
		}
	}

	static class BankHandler implements HttpHandler {
		// Stores the balance for each user
		private final Map<String, Double> users = new HashMap<>();

		@SuppressWarnings("unchecked")
		@Override
		public void handle(HttpExchange exchange) throws IOException {
			String response = "";
			if(exchange.getRequestURI().getPath().endsWith("register")) {
				StringBuilder buf = new StringBuilder();
				buf.append("<HTML><BODY><H1>Create Account</H1>");
				buf.append("<form name=\"register\" action=\"/bank\" method=POST>");
				buf.append("<TABLE>");
				buf.append("<TR><TD>Name:</TD> <TD><input size=40 maxlength=40 name=\"user\"></TD></TR>");
				buf.append("<TR><TD>Amount:</TD><TD><input size=40 maxlength=40 type=\"number\" step=0.01 name=\"amount\"></TD></TR>");
				buf.append("</TABLE>");
				buf.append("<p>");
				buf.append("<input name=\"submit\" type=submit value=\"Absenden\">");
				buf.append("</form>");
				buf.append("</body></html>");
				response = buf.toString();
			} else if ("GET".equals(exchange.getRequestMethod())) {
				StringBuilder buf = new StringBuilder();
				buf.append("<HTML><BODY><H1>Accounts</H1>");
				buf.append("<table border=1>");
				for(String name: users.keySet()) {
					buf.append(String.format("<tr><td width=300>%s</td><td width=100 align=right>%20.2f</td></tr>", name, users.get(name)));
				}
				buf.append("</table>");
				buf.append("<a href=\"bank/register\">create new account</a>");
				buf.append("</body></html>");
				response = buf.toString();
			} else if ("POST".equals(exchange.getRequestMethod())) {
				Map<String, Object> params = (Map<String, Object>) exchange.getAttribute("parameters");
				String user = (String)params.get("user");
				double amount = Double.parseDouble((String)params.get("amount"));
				users.put(user, amount);
				exchange.getResponseHeaders().add("Location", "/bank");
				exchange.sendResponseHeaders(301, -1);
				return;
			}
			exchange.getResponseHeaders().add("Content-type", "text/html; charset=UTF-8");
			exchange.sendResponseHeaders(200,0);
			OutputStream os = exchange.getResponseBody();
			os.write(response.getBytes(Charset.forName("UTF-8")));
			os.close();
		}
		
	}

	static class ParameterParser extends Filter {

		@Override
		public String description() {
			return "Parses the requested URI for parameters";
		}

		@Override
		public void doFilter(HttpExchange exchange, Chain chain)
				throws IOException {
			parseGetParameters(exchange);
			parsePostParameters(exchange);
			chain.doFilter(exchange);
		}

		private void parseGetParameters(HttpExchange exchange)
				throws UnsupportedEncodingException {
			Map<String, Object> parameters = new HashMap<>();
			URI requestedUri = exchange.getRequestURI();
			String query = requestedUri.getRawQuery();
			parseQuery(query, parameters);
			exchange.setAttribute("parameters", parameters);
		}

		private void parsePostParameters(HttpExchange exchange)
				throws IOException {
			if ("post".equalsIgnoreCase(exchange.getRequestMethod())) {
				@SuppressWarnings("unchecked")
				Map<String, Object> parameters = (Map<String, Object>) exchange
						.getAttribute("parameters");
				InputStreamReader isr = new InputStreamReader(
						exchange.getRequestBody(), "utf-8");
				BufferedReader br = new BufferedReader(isr);
				String query = br.readLine();
				parseQuery(query, parameters);
			}
		}

		@SuppressWarnings("unchecked")
		public static void parseQuery(String query,
				Map<String, Object> parameters)
				throws UnsupportedEncodingException {
			if (query != null) {
				StringTokenizer st = new StringTokenizer(query, "&");
				while (st.hasMoreTokens()) {
					String keyValue = st.nextToken();
					StringTokenizer st2 = new StringTokenizer(keyValue, "=");
					String key = null;
					String value = "";
					if (st2.hasMoreTokens()) {
						key = st2.nextToken();
						key = URLDecoder.decode(key, "UTF-8");
					}

					if (st2.hasMoreTokens()) {
						value = st2.nextToken();
						value = URLDecoder.decode(value, "UTF-8");
					}

					if (parameters.containsKey(key)) {
						Object o = parameters.get(key);
						if (o instanceof List) {
							List<String> values = (List<String>) o;
							values.add(value);
						} else if (o instanceof String) {
							List<String> values = new ArrayList<String>();
							values.add((String) o);
							values.add(value);
							parameters.put(key, values);
						}
					} else {
						parameters.put(key, value);
					}
				}
			}
		}
	}
}

```

## Client

```java
public class ClientGet {
	
	public static void main(String[] args) throws Exception {
		HttpClient client = HttpClient.newHttpClient();

		// the following lines create a HttpClient which follows redirects
		// client = HttpClient.newBuilder().followRedirects(Redirect.NORMAL).build();
		
		HttpRequest request = HttpRequest.newBuilder()
				.uri(new URI("http://www.fhnw.ch/de"))
				.GET()
				.build();

		HttpResponse<String> response = client.send(request, HttpResponse.BodyHandlers.ofString());

		System.out.println("Statuscode: " + response.statusCode());
		System.out.println("Headers:");
		response.headers().map().forEach((k, v) -> System.out.println(k + ": " + v));
		System.out.println("Body:");
		System.out.println(response.body());
	}

	public static void main(String[] args) throws Exception {
		HttpClient client = HttpClient.newHttpClient();

        BodyPublisher body = HttpRequest.BodyPublishers.ofString("user=Meyer2&amount=12345");
        
		HttpRequest request = HttpRequest.newBuilder()
				.uri(new URI("http://localhost:80/bank"))
				.POST(body)
				.build();

		HttpResponse<String> response = client.send(request, HttpResponse.BodyHandlers.ofString());

		System.out.println("Statuscode: " + response.statusCode());
		System.out.println("Headers:");
		response.headers().map().forEach((k, v) -> System.out.println(k + ": " + v));
		System.out.println("Body:");
		System.out.println(response.body());
	}
}
```


### Tomcat Server

```java
// Starts a tomcat server with a single web app accessible at http://localhost:8080/ds
public class StartServer {

    // server
    public static void main(String[] args) throws Exception {
        Tomcat tomcat = new Tomcat();
        tomcat.setPort(8080);
		tomcat.setBaseDir("build");

        // adds a webapp directory under a particular root name
        String webappDirLocation = "src/main/webapp/";
        StandardContext ctx = (StandardContext) tomcat.addWebapp("/ds", new File(webappDirLocation).getAbsolutePath());

        // adds  an alternative location for your "WEB-INF/classes" directory
        File additionWebInfClasses = new File("build/classes/java/main");
        WebResourceRoot resources = new StandardRoot(ctx);
        resources.addPreResources(
        		new DirResourceSet(
        				resources, 
        				"/WEB-INF/classes",	// /WEB-INF/lib/ for jars
        				 additionWebInfClasses.getAbsolutePath(), 
        				 "/"));
        ctx.setResources(resources);

		tomcat.getConnector();	// creates the default connector
        tomcat.start();
        tomcat.getServer().await();
    }

    // servlet
    public static void main(String[] args) throws LifecycleException {
		Tomcat tomcat = new Tomcat();
		tomcat.setPort(8080);
		tomcat.setBaseDir("build");

		@SuppressWarnings("serial")
		HttpServlet servlet = new HttpServlet() {
			@Override
			protected void doGet(HttpServletRequest req, HttpServletResponse resp)
					throws ServletException, IOException {

				PrintWriter writer = resp.getWriter();
				writer.println("<html><title>Welcome</title><body>");
				writer.println("<h1>Have a Great Day!</h1>");
				writer.println("</body></html>");
			}
		};

		// Analogous to the definition in web.xml
		String contextPath = "/simple";
		String servletName = "SimpleServlet";
		String urlPattern = "/go";
		Context context = tomcat.addContext(contextPath, new File(".").getAbsolutePath());
		tomcat.addServlet(contextPath, servletName, servlet);
		context.addServletMappingDecoded(urlPattern, servletName);

		tomcat.getConnector();	// creates the default connector
		tomcat.start();
		tomcat.getServer().await();
	}

    public static void main(String[] args) throws Exception {
		try (Socket s = new Socket("localhost", SHUTDOWN_PORT);
			 Writer w = new OutputStreamWriter(s.getOutputStream())) {
			w.write("SHUTDOWN\n");
		}
	}
}


```


## Messaging
### Server

```java
public class RabbitMq {

    public static final String QUEUE_NAME = "jku.bank.command.queue";
    public static final String EXCHANGE_NAME = "jku.bank.command.exchange";

    public static void main(String[] args) throws IOException, TimeoutException {

        ConnectionFactory factory = new ConnectionFactory();

        factory.setUsername("vesys-dev");
        factory.setPassword("vesys123");
        factory.setVirtualHost("vesys-dev-vhost");

        String host = "86.119.38.130";
        int port = 5672;

        factory.setHost(host);
        factory.setPort(port);

        Connection connection = factory.newConnection();

        Channel channel = connection.createChannel();
        channel.queueDeclare(QUEUE_NAME, false, false, false, null);
        channel.exchangeDeclare(EXCHANGE_NAME, "fanout");


        Bank bank = new Bank();
        DeliverCallback deliverCallback = (consumerTag, delivery) -> {
            AMQP.BasicProperties replyProps = new AMQP.BasicProperties.Builder()
                    .correlationId(delivery
                            .getProperties()
                            .getCorrelationId())
                    .build();


            try {

                Transfer transfer = Converter.deserialize(delivery.getBody());
                Transfer result = bank.transferCall(transfer);
                channel.basicPublish("", delivery.getProperties().getReplyTo(), replyProps, Converter.serialize(result));

            } catch (ClassNotFoundException e) {
                e.printStackTrace();
            }
        };

        channel.basicConsume(QUEUE_NAME, true, deliverCallback, (consumerTag -> {}));
    }
}

```

### Client

``` java
public class RabbitMqDriver implements Driver {

    SynchronousQueue<Transfer> queue = new SynchronousQueue<>();
    Channel channel;
    Connection connection;

    ConnectionFactory factory;
    String replyQueueName;

    @Override
    public void connect(String[] args) throws IOException {
        String host = args[0];
        int port = Integer.parseInt(args[1]);

        factory = new ConnectionFactory();
        factory.setUsername("vesys-dev");
        factory.setPassword("vesys123");
        factory.setVirtualHost("vesys-dev-vhost");
        factory.setHost(host);
        factory.setPort(port);

        try {
            connection = factory.newConnection();
            channel = connection.createChannel();
            channel.queueDeclare(QUEUE_NAME, false, false, false, null);
            replyQueueName = channel.queueDeclare().getQueue();

            channel.basicConsume(replyQueueName, true,
                    (consumerTag, message) -> {
                        try {
                            queue.put(Converter.deserialize(message.getBody()));
                        } catch (InterruptedException | ClassNotFoundException e) {
                            throw new RuntimeException(e);
                        }
                    },
                    tag -> {
                    }
            );
        } catch (TimeoutException e) {
            throw new IOException(e);
        }
    }

    @Override
    public void disconnect() throws IOException {

    }

    @Override
    public Transfer send(Transfer transfer) {
        try {

            AMQP.BasicProperties props = new AMQP.BasicProperties
                    .Builder()
                    .replyTo(replyQueueName)
                    .build();

            channel.basicPublish("", QUEUE_NAME, props, Converter.serialize(transfer));

        } catch (IOException e) {
            e.printStackTrace();
        }

        try {
            return (Transfer) queue.take();
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        }   
    }
}

```


## Rest


```java
/*

Resource /msg
=============
curl -X GET -i http://localhost:9998/msg
curl -X GET -i -H "Accept: text/plain"       http://localhost:9998/msg
curl -X GET -i -H "Accept: application/json" http://localhost:9998/msg
curl -X GET -i -H "Accept: application/xml"  http://localhost:9998/msg
curl -X GET -i -H "Accept: application/xstream" http://localhost:9998/msg

curl -X DELETE   http://localhost:9998/msg

curl -X PUT -i -H "Content-Type: application/json" --data "{\"text\":\"JSON message\"}" http://localhost:9998/msg
curl -X PUT -i -H "Content-Type: application/xml"  --data "<msg><text>XML message</text></msg>" http://localhost:9998/msg
curl -X PUT -i -H "Content-Type: application/xstream"  --data "<ch.fhnw.ds.rest.msg.data.Msg><text>XStream message</text></ch.fhnw.ds.rest.msg.data.Msg>" http://localhost:9998/msg

curl -X PUT -i -H "Content-Type: text/plain" --data "msg=Plain Text message" http://localhost:9998/msg
curl -X PUT -i     --data msg=Hallo http://localhost:9998/msg
 
curl -X POST -i -H "Content-Type: application/json" --data "{\"text\":\"json\"}" http://localhost:9998/msg

Resource /msg/{id}
==================
curl -X GET -i http://localhost:9998/msg/Dominik
curl -X GET -i http://localhost:9998/msg/Dominik/headers


curl -X OPTIONS -i http://localhost:9998/msg
curl --head -i http://localhost:9998/msg

*/
public static void main(String[] args) throws Exception {
    URI baseUri = new URI("http://localhost:9998/");
		
	// @Singleton annotations will be respected 
	ResourceConfig rc = new ResourceConfig(MsgResource.class);

	// Create and start the JDK HttpServer with the Jersey application
	JdkHttpServerFactory.createHttpServer(baseUri, rc);
}
    

@Singleton
@Path("/msg")
public class MsgResource {

	private XStream xstream = new XStream();

	public MsgResource() {
		System.out.println("MsgResource() called");
	}

	@GET
	@Produces("text/plain")
	public String getPlain() {
		return msg + "\n";
	}

	@GET
	@Produces("text/html")
	public String getHtml() {
		StringBuffer buf = new StringBuffer();
		buf.append("<html><body><h1>Message Text</h1>" + msg + "<br>");
		buf.append("<form method=\"POST\" action=\"/msg\">");
		buf.append("<p>Text: <input name=\"msg\" type=\"text\" size=20/>");
		buf.append("<input type=\"submit\" value=\"Submit\" />");
		buf.append("</form>");
		buf.append("</body></html>");
		return buf.toString();
	}

	@GET
	@Produces("application/xstream")
	public String getXml() {
		return xstream.toXML(new Msg(msg));
	}

	@GET
	@Produces({"application/json", "application/xml"})
	public Msg getJson() {
		return new Msg(msg);
	}

	@PUT
	@Consumes("text/plain")
	public void setTextPlain(String msg) {
		this.msg = msg;
	}

	@PUT
	@Consumes( { "application/json", "application/xml" } )
	public void setTextJson(Msg message) {
		msg = message.getText();
	}

	@PUT
	@Consumes( "application/xstream" )
	public void setTextXml(String text) {
		Msg message = (Msg)xstream.fromXML(text);
		msg = message.getText();
	}

	@PUT
	@Consumes("application/x-www-form-urlencoded")
	@Produces("text/xml")
	public String setTextForm(@FormParam("msg") String msg) {
		this.msg = msg;

		ByteArrayOutputStream stream = new ByteArrayOutputStream();
		XMLEncoder enc = new XMLEncoder(stream);
		enc.writeObject(msg);
		enc.close();
		return new String(stream.toByteArray()) + "\n";
	}

	// POST on /msg (used for forms)
	// =============================

	@POST
	@Consumes("application/x-www-form-urlencoded")
	@Produces("text/html")
	public String doPost(@FormParam("msg") String msg) {
		this.msg = msg;
		return getHtml();
	}

	@POST
	@Consumes("application/x-www-form-urlencoded")
	@Produces("application/json")
	public Msg doPost2(@FormParam("msg") String msg) {
		this.msg = msg;
		return new Msg(this.msg);
	}

	@POST
	@Consumes( { "application/xml", "application/json" })
	public Response createNewMessage(@Context UriInfo uriInfo, Msg message) {
		URI location = uriInfo.getAbsolutePathBuilder().path(message.getText()).build();
		return Response.created(location).build();
	}

	// DELETE on /msg
	// ==============

	@DELETE
	@Produces("text/plain")
	public String onDelete() {
		msg = null;
		return "Message deleted.\n";
	}

	// GET on /msg/cc
	// ==============

	@GET
	@Path("cc")
	@Produces("text/plain")
	public Response getPlain2() {
		ResponseBuilder builder = Response.ok(msg + "\n");
		CacheControl cc = new CacheControl();
		cc.setMaxAge(1000); // HTTP max-age field, in seconds
		cc.setNoTransform(true);
		cc.setPrivate(true);
		builder.cacheControl(cc);
		return builder.build();
	}

	// GET on /msg/{id} and on /msg/{id}/headers
	// =========================================

	@GET
	@Produces("text/plain")
	@Path("{id}")
	public String readDetailsInfo(@PathParam("id") String path, @Context Request r) {
		return msg + ": " + path + "\n";
	}

	@GET
	@Produces("text/plain")
	@Path("{id}/headers")
	public String readDetailHeaders(@PathParam("id") String path,
			@Context HttpHeaders headers) {
		StringBuffer buf = new StringBuffer();
		buf.append("Headers of request " + path + "\n\n");
		MultivaluedMap<String, String> map = headers.getRequestHeaders();
		for (String key : map.keySet()) {
			buf.append(key + ": " + map.getFirst(key) + "\n");
		}
		return buf.toString();
	}
}
```

## ETAG

- `evaluatePreconditions(EntityTag eTag)`
- `evaluatePreconditions(Date lastModified)`
- `evaluatePreconditions(Date lastModified, EntityTag eTag)`

- `If-Modified-Since: Tue, 5 Mar 2020 09:15 EST`
- `If-None-Match: "3145346346476787653144623"`
- `If-Unmodified-Since: Tue, 5 Mar 2020 09:15 EST`
- `If-Match: "3145346346476787653144623`

**Optimistic Locking**

	If-Unmodified-Since: Tue, 6 Mar 2020 09:15 EST
	If-Match: "3145346346476787653144623"

# Websocket

## Handshake

	GET /examples/websocketechoStream HTTP/1.1
	Host: server.example.com
	Connection: Upgrade
	Upgrade: websocket
	Sec-Websocket-Key: mqn5Pm7wtXEX6BzqDInLjw==
	Sec-Websocket-Version: 13

---

	HTTP/1.1 101 Switching Protocols
	Server: Apache-Coyote/1.1
	Upgrade: websocket
	Connection: upgrade
	Sec-WebSocket-Accept: +TdGPOkAq62+toDOhVGj2QZWwg8=
	Date: Thu, 04 Apr 2013 19:21:39 GMT

	# Subprotocolls
	Sec-WebSocket-Protocol: chat, superchat
	Sec-WebSocket-Protocol: chat

## Masking

	0x81	1000	0001	Final Fragment | Text frame
	0x85	1000	0101	Masked / length = 5
	0x96	1001	0110	Masking Key
	0xa7	1010	0111	Masking Key
	0x2b	0010	1011	Masking Key
	0x38	0011	1000	Masking Key
	0xde	1101	1110	xor 1001 0110 = 0100 1000 H
	0xc2	1100	0010	xor 1010 0111 = 0110 0101 e
	0x47	0100	0111	xor 0010 1011 = 0110 1100 l
	0x54	0101	0100	xor 0011 1000 = 0110 1100 l
	0xf9	1111	1001	xor 1001 0110 = 0110 1111 o


```java
@ServerEndpoint("/echo")
// public class EchoServer extends Endpoint 

public class EchoServer {
	
	public EchoServer(){
		System.out.println("EchoServer created " + this);
	}

	public static void main(String[] args) throws Exception {
		Server server = new Server("localhost", 2222, "/websockets", null, EchoServer.class);
		server.start();
		System.out.println("Server started, press a key to stop the server");
		System.in.read();
	}
	
	@OnOpen
	public void onOpen(Session session) {
		System.out.printf("New session %s\n", session.getId());
	}

	@OnClose
	public void onClose(Session session, CloseReason closeReason) {
		System.out.printf("Session %s closed because of %s\n", session.getId(), closeReason);
	}

	@OnMessage
	public String onMessage(String message, Session session) {
		System.out.println("received message form " + session.getBasicRemote() + ": " + message);
		return "echo " + message;
	}

	@OnError
	public void onError(Throwable exception, Session session) {
		System.out.println("an error occured on connection " + session.getId() + ":" + exception);
	}

}
```

```java
public class EchoClient extends Endpoint {

	private static CountDownLatch latch = new CountDownLatch(1);

	SynchronousQueue<Transfer> queue = new SynchronousQueue<>();


	@Override
	public void onOpen(final Session session, EndpointConfig config) {
		System.out.printf("onOpen [%s]\n", Thread.currentThread());
		session.addMessageHandler(new MessageHandler.Whole<String>() {
			@Override
			public void onMessage(String message) {
				System.out.printf("onMessage [%s] %s\n", Thread.currentThread(), message);
				try {
					session.close();
				} catch (IOException e) {
					throw new RuntimeException(e);
				}
			}
        });

		try {
			session.getBasicRemote().sendText("Hello");
		} catch (IOException e) {
			throw new RuntimeException(e);
		}
	}

	@Override
	public void onClose(Session session, CloseReason closeReason) {
		System.out.printf("onClose [%s] Session %s closed because of %s\n", Thread.currentThread(), session.getId(), closeReason);
		latch.countDown();
	}

	@Override
	public void onError(Session session, Throwable exception) {
		System.out.printf("onError [%s] an error occured on connection %s: %s\n", Thread.currentThread(), session.getId(), exception);
	}


	public static void main(String[] args) throws Exception {
		URI url = new URI("ws://localhost:2222/websockets/echo");

		System.out.println(Thread.currentThread());

		ClientManager client = ClientManager.createClient();
		ClientEndpointConfig cec = ClientEndpointConfig.Builder.create().build();

//		client.connectToServer(EchoClient.class, cec, url);
		client.connectToServer(new EchoClient(), cec, url);
		latch.await();
	}

}
```


# GraphQL
## Schema



```typescript
type Customer {
	id: ID!
	name: String!
	ratings: [Rating]!
}

type Rating {
	id: ID!
	product: Product
	customer: Customer
	score: Int!
	comment: String!
}

type Query {
	products : [Product!]!
	product(id: ID!): Product
}


input ProductInput {
	title: String!
	description: String!
	imageUrl: String!
}

type Mutation {
	rateProduct(productId: ID!, customerId: ID!, score: Int!, comment: String!) : Rating!

	createProduct(product: ProductInput!) : Product!
	updateProduct(id: ID!, product: ProductInput!) : Product
}

schema {
	query: Query
	mutation: Mutation
}

```

```java
@Component
public class Mutation implements GraphQLMutationResolver {

	@Autowired
	private ShopRepository shopRepository;

	public Product createProduct(@NotNull ProductInput product) {
		return shopRepository.createProduct(product.getTitle(), product.getDescription(), product.getImageUrl());
	}

	public Product updateProduct(String id, ProductInput productInput) {
		return null;
	}

	public Rating rateProduct(String productId, String customerId, int score, String comment) {
		return shopRepository.rateProduct(productId, customerId, score, comment);
	}
}

@Component
public class Query implements GraphQLQueryResolver {

	@Autowired
	private ShopRepository shopRepository;

	public Collection<Product> products() {
		return shopRepository.getAllProducts();
	}

	public Optional<Product> product(String id) {
		return shopRepository.getProductById(id);
	}
}

```

## Akka
### Hashtable
 
```java
public class HashNode extends AbstractActor {
	private final int id;
	
	private int next;
	private TreeMap<Integer, ActorRef> fingerTable;
	
	private Map<Object, Object> values = new HashMap<>();

	public HashNode(int id) { this.id = id; }

	@Override
	public void preStart() throws Exception {
		getContext()
				.setReceiveTimeout(Duration.create(5, TimeUnit.SECONDS));
		super.preStart();
	}

	@SuppressWarnings("unchecked")
	@Override
	public Receive createReceive() {
		return receiveBuilder()
			.match(TreeMap.class, fingerTable -> {
				this.fingerTable = fingerTable;
			})
			.match(Integer.class, next -> {
				this.next = next;
			})
			.match(Put.class, msg -> {
				int hash = msg.key.hashCode();
				//System.out.printf("Put of %s at %s%n", hash, id);
				if(between(hash, id, next)) {
					fingerTable
							.get(next)
							.tell(new Put2(msg.key, msg.value), getSelf());
				} else {
					var set = fingerTable.navigableKeySet();
					var prev = set.lower(hash);
					if(prev == null) prev = set.last();
					fingerTable.get(prev).tell(msg, getSelf());
				}
			})
			.match(Put2.class, msg -> {
				values.put(msg.key, msg.value);
			})
			.match(Get.class, msg -> {
				int hash = msg.key.hashCode();
				//System.out.printf("Lookup of %s at %s%n", hash, id);
				if(between(hash, id, next)) {
					fingerTable.get(next).tell(new Get2(msg.key, msg.counter + 1), getSender());
				} else {
					var set = fingerTable.navigableKeySet();
					var prev = set.lower(hash);
					if(prev == null) prev = set.last();
					fingerTable.get(prev).tell(new Get(msg.key, msg.counter + 1), getSender());
				}
			})
			.match(Get2.class, msg -> {
				getSender().tell(new Result(this.id, msg.key, values.get(msg.key), msg.counter), getSelf());
			})
			.match(Partition.class, msg -> {
				Map<Object, Object> res = new HashMap<>();
				var it = values.entrySet().iterator();
				while(it.hasNext()) {
					var entry = it.next();
					var hash = entry.getKey().hashCode();
					if(!between(hash, msg.id, this.id)) {
						res.put(entry.getKey(), entry.getValue());
						it.remove();
					}
				}
				getSender().tell(new PartitionAnswer(res), getSelf());
			})
			.match(PartitionAnswer.class, msg -> {
				System.out.printf("added %d values to node %d%n", msg.map.size(), id);
				values.putAll(msg.map);
			})
			.match(Print.class, msg -> {
				synchronized(System.out) { 
					System.out.println("Node " + id);
					System.out.println("\tnext: " + next);
					System.out.println("\tfingertable:");
					for(var e : fingerTable.entrySet()) {
						System.out.printf("\t\t%s: %s%n", e.getKey(), e.getValue());
					}
					for(Object key: values.keySet()) {
						System.out.printf("\t%s [%d] => %s%n", key, key.hashCode(), values.get(key));
					}
				}
			})
			.match(ReceiveTimeout.class, msg -> {
				// TODO the old entries shoudl be removed
				ActorRef ref = fingerTable.get(next);
				int dx = 1;
				for(int k = 0; k < HashTest.K; k++) {
					final int hash = id+dx;
					ref.tell(new Get(new Object() {
						public int hashCode() { return hash; }
					}), getSelf());
					dx = dx << 1;
				}
			})
			.match(Result.class, msg -> {
				fingerTable.put(msg.id, getSender());
			})
			.matchAny(msg -> {
				System.out.println("UnHandled Message Received");
				unhandled(msg);
			})
			.build();
	}
}

public class Main{
    public static void main(String[] args) throws Exception {
        ActorSystem as = ActorSystem.create();
        Random r = new Random();

        TreeMap<Integer, ActorRef> actors = new TreeMap<>();
        for (int i = 0; i < N; i++) {
            int id = r.nextInt();
            actors.put(id, as.actorOf(Props.create(HashNode.class, id), "Node:" + id));
        }

        ActorRef a0 = actors.firstEntry().getValue();
        System.out.println("a0 = " + a0);

        var keySet = actors.navigableKeySet();
        for (int key : keySet) {
            var fingerTable = getFingerTable(actors, key);
            actors
                    .get(key)
                    .tell(fingerTable, null);
        }

        Timeout timeout = new Timeout(5, TimeUnit.SECONDS);
        BufferedReader stdin = new BufferedReader(new InputStreamReader(System.in));
		boolean exit = false;
	}
}
```
