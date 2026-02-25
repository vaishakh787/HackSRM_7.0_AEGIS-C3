from dotenv import load_dotenv
load_dotenv()

import os
from flask import Flask
from routes.events import events_bp

app = Flask(__name__)
app.register_blueprint(events_bp)

@app.route("/health")
def health():
    return {"status": "ok"}, 200

if __name__ == "__main__":
    port = int(os.getenv("FLASK_PORT", 5000))
    print(f"[AEGIS Backend] Running on 0.0.0.0:{port}")
    app.run(host="0.0.0.0", port=port, debug=True)