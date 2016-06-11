var flag = false;

function super_button_animation(divs) {
    var i;
    if (flag)
    {
	flag = false;
    }
    else
    {
	for (i = 0; i < divs.length; i++)
	{
	    var delay = parseFloat(0.15 * i) + "s";
	    console.log(delay);
	    $(divs[i]).css({"display":"inherit", "animation-delay":delay, "-webkit-animation-delay":delay}).addClass('fadeInLeft').on('animationend webkitAnimationEnd MSAnimationEnd oAnimationEnd', function(e) {
		$(this).removeClass('fadeInLeft');
	    });
	}
	flag = true;
    }
}

function plus_btn_clicked() {
    $('#plus_btn').click(function(){
	var div = $('.super_button');
	super_button_animation(div);
	$(this).addClass('rotate_mill').on('animationend webkitAnimationEnd MSAnimationEnd oAnimationEnd', function(e) {
            $(this).removeClass('rotate_mill');
        });
    });
}

$(document).ready(function(){
    plus_btn_clicked();
});

