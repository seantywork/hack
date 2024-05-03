let net = require("net");
let tls = require("tls");
let fs = require("fs");


const config = require("./config");


class server  {

    constructor(PORT){
  
      this.PORT = PORT
  
      this.PLAINSOCK = net.createServer();
  
  
      this.PLAINSOCK.on("connection", function(sconn){
  
        let remote_address = sconn.remoteAddress

  
        sconn.on("data", async function(data){
    

          console.log(data)

          sconn.write("received" + data.length)
        
  
        })
    
        sconn.on("close",function(data){
    
  
          console.log("net client going away")
          
    
        })
    
      })
  
  
      this.do_accept();
  
    }
  
    do_accept = function (){
  
  
      this.PLAINSOCK.listen(this.PORT, "0.0.0.0");

  
      console.log("net server bound and listening")
    }
  
  
  }
  
  
  
  class ssl_server {
  
    constructor(PORT){
  
      this.PORT = PORT
  
      this.tls_options = {
        key: fs.readFileSync(config.ROOT_CA_KEY_PATH),
        cert: fs.readFileSync(config.ROOT_CA_PATH),
      
      //  ca: [ fs.readFileSync('client-cert.pem') ]
      };
  
      this.SECURESOCK = tls.createServer(this.tls_options, function(sconn){
  

        let remote_address = sconn.remoteAddress
  
        sconn.on("data", async function(data){
    

  
            console.log(data)

            sconn.write("received" + data.length)          
          
  
  
        })
    
        sconn.on("close",function(data){
    
          console.log("tls client going away")
          
        })
    
    
      });
  
      this.do_accept();
  
    }
  
    do_accept = function (){
  
      this.SECURESOCK.listen(this.PORT, "0.0.0.0");

      console.log("tls server bound and listening")
  
    }
  
  }
  
  
  
  