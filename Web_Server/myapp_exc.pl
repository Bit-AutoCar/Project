#!/usr/bin/env perl

use v5.14;
use utf8;

use Mojolicious::Lite;
use Mojo::Log;

my $log = Mojo::Log->new;

get '/' => sub {
    my $self = shift;

		$self->redirect_to( $self->url_for('/drive') );
};

get '/drive' => sub {
  my $self = shift;

  $self->render('drive');
};

post '/drive' => sub {
    my $self = shift;

		my $course;
		$course = $self->param('course');

		`gpio -g mode 17 out`;
		`gpio -g mode 18 out`;
		`gpio -g mode 22 out`;
		`gpio -g mode 27 out`;
		`gpio -g mode 23 out`;
		`gpio -g mode 24 out`;
		`gpio -g write 23 1`;
		`gpio -g write 24 1`;

# 12 -> 17(오른 1 전방), 16 -> 18 (오른쪽 후진) 17:1 18:0 전진 17:0 18:1 후진
# 20 -> 22, 21-> 27  22:1 27:0 전진 22:0 27:1 후진

		if ($course eq 'Go') {
			`gpio -g write 22 1`;
			`gpio -g write 27 0`;
			`gpio -g write 17 1`;
			`gpio -g write 18 0`;
		}
		elsif ($course eq 'Left') {
			`gpio -g write 22 0`;
			`gpio -g write 27 1`;
			`gpio -g write 17 1`;
			`gpio -g write 18 0`;
		}
		elsif ($course eq 'Right') {
			`gpio -g write 22 1`;
			`gpio -g write 27 0`;
			`gpio -g write 17 0`;
			`gpio -g write 18 1`;
		}
		elsif ($course eq 'Back') {
			`gpio -g write 22 0`;
			`gpio -g write 27 1`;
			`gpio -g write 17 0`;
			`gpio -g write 18 1`;
		}
		elsif ($course =~ 'Stop') {
			`gpio -g write 22 0`;
			`gpio -g write 27 0`;
			`gpio -g write 17 0`;
			`gpio -g write 18 0`;
		}
};

app->start;

__DATA__

@@ layouts/default.html.ep
<!DOCTYPE html>
<html>
  <head>
    <title><%= title %></title>
		<script src="https://ajax.googleapis.com/ajax/libs/jquery/1.11.3/jquery.min.js"></script>
  </head>
  <body>
    <%= content %>
  </body>
</html>

@@ drive.html.ep
% layout 'default';
% title 'DRIVE';

<p>TEST</p>
<form id="myForm" action="/drive" method="post">
	<div align="center" style="margin: 0 0 10px 10px">
      <ul>
        <input type="button" id="go" name="go" style=font-size: 20pt; width:70; height:60; value="GO">
      </ul>
      <ul>
        <input type="button" id="left" style=font-size: 20pt; width:70; height:60; value="LEFT" >
        <input type="button" id="stop" style=font-size: 20pt; width:70; height:60; value="STOP" >
        <input type="button" id="right" style=font-size: 20pt; width:70; height:60; value="RIGHT" >
      </ul>
      <ul >
        <input type="button" id="back" style=font-size: 20pt; width:70; height:60; value="BACK" >
      </ul>
	</div>
</form>

<script>
	$(document).ready(function() {
			$("input[type=button]").click(function() {
				$.post("/drive", {course: $(this).val()});
				});
	});
</script>


