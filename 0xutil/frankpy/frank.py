from flask import Flask
from flask_session import Session
from flask_cors import CORS


from urls import index as urls_index


app = Flask(__name__)
port = 8000
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)
CORS(app)

app.static_folder = 'static'

app = urls_index.Init(app)


if __name__ == "__main__":
    
    app.run(host='0.0.0.0',port=port,debug=False)
