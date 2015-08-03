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

		use HiPi::BCM2835;
		use HiPi::Device::GPIO;
		use HiPi::Constant qw( :raspberry );

		my $bcm = HiPi::BCM2835->new();

		$bcm->gpio_write(23, 1);
		$bcm->gpio_write(24, 1);

# 12 -> 17, 16 -> 18
# 20 -> 22, 21-> 27 

		if ($course eq 'GO') {
			$bcm->gpio_write(17, 1);
			$bcm->gpio_write(18, 0);
			$bcm->gpio_write(22, 1);
			$bcm->gpio_write(27, 0);
		}
		elsif ($course eq 'LEFT') {
			$bcm->gpio_write(17,1);
			$bcm->gpio_write(18,0);
			$bcm->gpio_write(22,0);
			$bcm->gpio_write(27,1);
		}
		elsif ($course eq 'RIGHT') {
			$bcm->gpio_write(17,0);
			$bcm->gpio_write(18,1);
			$bcm->gpio_write(22,1);
			$bcm->gpio_write(27,0);
		}
		elsif ($course eq 'BACK') {
			$bcm->gpio_write(17,0);
			$bcm->gpio_write(18,1);
			$bcm->gpio_write(22,0);
			$bcm->gpio_write(27,1);
		}
		elsif ($course eq 'STOP') {
			$bcm->gpio_write(17, 0);
			$bcm->gpio_write(18, 0);
			$bcm->gpio_write(22, 0);
			$bcm->gpio_write(27, 0);

			$bcm->gpio_write(23, 0);
			$bcm->gpio_write(24, 0);
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


