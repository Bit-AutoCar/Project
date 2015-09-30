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

	my $GBgauge;
	if ($self->param('GBgauge'))
	{
		my $GBgear = '1';
		$GBgauge = $self->param('GBgauge');

		if ($GBgauge eq '1') {
			$GBgear = 'a'; #97
		}
		elsif ($GBgauge eq '2') {
			$GBgear = 'b'; #98
		}
		elsif ($GBgauge eq '3') {
			$GBgear = 'c'; #99
		}
		elsif ($GBgauge eq '4') {
			$GBgear = 'd'; #100
		}

		$log->debug("GBgauge : $GBgauge : $GBgear");
		$Arduino->communicate($GBgear);
	}

	my $course = '';
	if($self->param('course'))
	{
		$course = $self->param('course');
		if ($course eq 'Go') {
			$Arduino->communicate(1); #49
			$log->debug("Go");
		}
		elsif ($course eq 'Back') {
			$Arduino->communicate(2); #50
			$log->debug("Back");
		}
		elsif ($course eq 'Left') {
			$Arduino->communicate(4); #52
			$log->debug("Left");
		}
		elsif ($course eq 'Right') {
			$Arduino->communicate(6); #54
			$log->debug("Right");
		}
		elsif ($course =~ 'LRStop') {
			$Arduino->communicate(7); #55
			$log->debug("LRStop");
		}
		elsif ($course =~ 'GBStop') {
			$Arduino->communicate(8); #56
			$log->debug("GBStop");
		}
		else {
			$Arduino->communicate(7); #55
			$Arduino->communicate(8); #56
			$log->debug("All STOP");
		}
	}

	my $power = $self->param('Power');
	if($power)
	{
		my $power_value = chr($power);
		$Arduino->communicate($power_value);
		$log->debug("power [$power_value]");
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


