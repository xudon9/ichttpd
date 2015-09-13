#!/usr/bin/python3
from string import Template
from to_human_time import to_human_time

def start_response(resp="text/html"):
#def start_response(resp="multipart/mixed"):
    return('Content-type: ' + resp + '\n\n')

def include_header(the_title = ''):
    with open('templates/header.html') as headf:
        head_text = headf.read()
    header = Template(head_text)
    return(header.substitute(title=the_title))

def include_footer(the_wishes):
    with open('templates/footer.html') as footf:
        foot_text = footf.read()
        wish_box_template = Template(foot_text)
        code_str = ''

    footer = Template(foot_text)
    return(footer.substitute(wishes="tmp string"))

def start_form(the_url, form_type="POST"):
    return('<form action="' + the_url + '" method="' + form_type + '">')

def end_form(submit_msg="Submit"):
    return('<p></p><input type=submit value="' + submit_msg + '">')

def radio_button(rb_name, rb_value):
    return('<input type="radio" name="' + rb_name +
                             '" value="' + rb_value + '"> ' + rb_value + '<br />')

def radio_button_id(rb_name, rb_value, rb_id):
    return('<input type="radio" name="' + rb_name +
                             '" value="' + str(rb_id) + '"> ' + rb_value + '<br />')

def u_list(items):
    u_string = '<ul>'
    for item in items:
        u_string += '<li>' + item + '</li>'
    u_string += '</ul>'
    return(u_string)

def header(header_text, header_level=2):
    return('<h' + str(header_level) + '>' + header_text +
           '</h' + str(header_level) + '>')

def para(para_text):
    return('<p>' + para_text + '</p>')

def create_inputs(inputs_list):
    html_inputs = ''
    for each_input in inputs_list:
        html_inputs = html_inputs + '<input type="Text" name="' + each_input + '" size=40>'
    return(html_inputs)
  
def include_footer(wishes_code):
    with open('templates/footer.html') as footf:
        foot_text = footf.read()
    footer = Template(foot_text)
    return(footer.substitute(wish_boxes=wishes_code))
  
def include_wishes_form(the_wishes):
    code_str = ''
    with open('templates/form.html') as formf:
        form_text = formf.read()
        box_template = Template(form_text)
        for i in the_wishes:
            code_str+=box_template.substitute(info_id = i[0], title = i[2],
                    name = 'By '+i[1], thumb_up = i[6], time = to_human_time(i[5]),
                    wish = i[3], contact_info = i[4])
    return(code_str)
