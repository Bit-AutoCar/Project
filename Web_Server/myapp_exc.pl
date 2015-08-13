#!/usr/bin/env perl

use v5.14;
use utf8;

use Device::SerialPort::Arduino;
use Mojolicious::Lite;
use Mojo::Log;

my $log = Mojo::Log->new;

get '/' => sub {
    my $self = shift;

		$self->redirect_to( $self->url_for('/drive') );
};





my $Arduino = Device::SerialPort::Arduino->new(
				port     => '/dev/ttyAMA0',
				baudrate => 115200,
			);

get '/drive' => sub {
	my $self = shift;

	$self->render('drive');
};

post '/drive' => sub {
    my $self = shift;

		my $course;
		$course = $self->param('course');

		$Arduino->communicate(1);
# 12 -> 17(오른 1 전방), 16 -> 18 (오른쪽 후진) 17:1 18:0 전진 17:0 18:1 후진
# 20 -> 22, 21-> 27  22:1 27:0 전진 22:0 27:1 후진

		if ($course eq 'Go') {
			$Arduino->communicate(1);
			$log->debug("Go");
		}
		elsif ($course eq 'Back') {
			$Arduino->communicate(2);
			$log->debug("Back");
		}
		elsif ($course eq 'Left') {
			$Arduino->communicate(3);
			$log->debug("Left");
		}
		elsif ($course eq 'Right') {
			$Arduino->communicate(4);
			$log->debug("Right");
		}
		elsif ($course =~ 'RLStop') {
			$Arduino->communicate(5);
			$log->debug("RLStop");
		}
		elsif ($course =~ 'GBStop') {
			$Arduino->communicate(6);
			$log->debug("GBStop");
		}
		else {
			$Arduino->communicate(5);
			$Arduino->communicate(6);
			$log->debug("All STOP");
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


