"""
The flask application package.
"""

from flask import Flask
app = Flask(__name__)
app.config.from_object(__name__)
import printer.views
