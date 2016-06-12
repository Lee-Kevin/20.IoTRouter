# coding = utf-8

from datetime import datetime
from printer import app

from flask import render_template, redirect, url_for
from wtforms import TextAreaField
from wtforms.validators import DataRequired
from flask_wtf import Form
SECRET_KEY = 'secret'
FILEPATH1   = "/tmp/message"
FILEPATH2   = "/tmp/printFlag"
def savefile(path,data):
    fs = open(path,"wb")
    fs.write(data)
    fs.close()

class CommentForm(Form):
    comment = TextAreaField("Message", validators=[DataRequired()])

@app.route('/')
@app.route('/home')
def home(form=None):
    """Renders the home page."""
    if form is None:
        form = CommentForm(csrf_enabled=False)
    return render_template(
        'index.html',
        title='Home Page',
        year=datetime.now().year,
        form=form,
    )

@app.route('/contact')
def contact():
    """Renders the contact page."""
    return render_template(
        'contact.html',
        title='Router Printer',
        year=datetime.now().year,
        message='seeedstudio.com'
    )

@app.route('/about')
def about():
    """Renders the about page."""
    return render_template(
        'about.html',
        title='About',
        year=datetime.now().year,
        message=''
    )

@app.route("/add/", methods=("POST",))
def add_comment():
    form = CommentForm(csrf_enabled=False)
    if form.validate_on_submit():
        try:
            savefile(FILEPATH1,form.comment.data)
            savefile(FILEPATH2,"1")
            print(form.comment.data)
        except UnicodeEncodeError:
            print("Please input ascii-code")
        except IOError:
            print("IOError")
        return redirect(url_for("home"))
    print("Something Wrong")
    return home(form)

