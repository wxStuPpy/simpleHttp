###  //http数据块示例 1.请求行 2.请求头 3.空行 4.客户端向服务器提交的数据 \r\n结尾
### GET 请求示例
假设你想从服务器获取一个网页的内容，比如访问 `https://example.com/index.html`。

客户端会构造一个 GET 请求，大致如下：
```
GET /index.html HTTP/1.1
Host: example.com
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9
Accept-Language: en-US,en;q=0.9
Connection: keep-alive
```
- `GET` 是请求方法，表示要获取资源。
- `/index.html` 是请求的资源路径。
- `HTTP/1.1` 是使用的 HTTP 协议版本。
- `Host` 指明请求的目标主机。
- `User-Agent` 标识客户端的类型和版本等信息。
- `Accept` 告诉服务器客户端能接受的响应内容类型。
- `Accept-Language` 表明客户端偏好的语言。
- `Connection` 说明希望与服务器保持持久连接。

服务器接收到这个 GET 请求后，会进行处理，若请求的资源存在且权限等都没问题，会返回一个响应，例如：
```
HTTP/1.1 200 OK
Date: Fri, 23 Mar 2025 12:00:00 GMT
Server: Apache/2.4.41 (Ubuntu)
Content-Type: text/html; charset=UTF-8
Content-Length: 5678
Connection: keep-alive

<!DOCTYPE html>
<html>
<head>
    <title>Example Page</title>
</head>
<body>
    <h1>Welcome to the Example Page</h1>
    <p>This is a sample HTML page.</p>
</body>
</html>
```
- `HTTP/1.1` 是协议版本，`200` 是状态码表示请求成功，`OK` 是状态消息。
- `Date` 是响应生成的时间。
- `Server` 标识处理请求的服务器软件。
- `Content-Type` 说明响应内容的类型和字符编码。
- `Content-Length` 是响应体的长度。
- 空行后面的 HTML 内容就是响应体，即请求的网页内容。

### POST 请求示例
假设你在一个网站的登录页面输入用户名和密码进行登录，客户端会构造一个 POST 请求来提交数据。

```
POST /login.php HTTP/1.1
Host: example.com
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36
Accept: application/json
Content-Type: application/json
Content-Length: 46

{"username": "your_username", "password": "your_password"}
```
- `POST` 是请求方法，表示要提交数据。
- `/login.php` 是处理登录请求的脚本路径。
- `Content-Type: application/json` 表明请求体的数据格式是 JSON。
- `Content-Length` 指明请求体的长度。
- 空行后面的 JSON 数据就是请求体，包含了用户名和密码信息。

服务器接收到 POST 请求后，会对请求体中的数据进行处理，比如验证用户名和密码是否正确，然后返回响应，例如：
```
HTTP/1.1 200 OK
Date: Fri, 23 Mar 2025 12:05:00 GMT
Server: Apache/2.4.41 (Ubuntu)
Content-Type: application/json
Content-Length: 56
Connection: keep-alive

{"message": "Login successful", "user_id": 12345}
```
- 响应的格式与 GET 请求的响应类似，响应体中包含了服务器处理请求后的结果信息，这里表示登录成功并返回了用户 ID。 