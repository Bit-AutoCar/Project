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

		my $GBgauge;
		$GBgauge = $self->param('GBgauge');

		my $RLgauge;
		$RLgauge = $self->param('RLgauge');
		$log->debug("------------ Course --------------");
		$log->debug("------------ <<$course>> --------------");
		$log->debug("------------ GBgauge --------------");
		$log->debug("------------ [[$GBgauge]] --------------");
		$log->debug("------------ RLgauge --------------");
		$log->debug("------------ (($RLgauge)) --------------");
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


