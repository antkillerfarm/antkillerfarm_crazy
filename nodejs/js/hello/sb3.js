var flag = false;
var animation_cnt = 0;

function super_button_animation(divs) {
    var i;
    var delay;

    for (i = 0; i < divs.length; i++)
    {
	delay = parseFloat(0.15 * i) + "s";
	if (flag)
	{
	    $(divs[i]).css({"display":"inherit", "animation-delay":delay, "-webkit-animation-delay":delay}).addClass('fadeOutLeft');
	}
	else
	{
	    $(divs[i]).css({"display":"inherit", "animation-delay":delay, "-webkit-animation-delay":delay}).addClass('fadeInLeft');
	}
	animation_cnt++;
	console.log(animation_cnt + ":" + delay);
    }
}

function plus_btn_clicked() {
    $('#plus_btn').click(function(){
	if (animation_cnt != 0)
	{
	    return;
	}
	var div = $('.super_button');
	super_button_animation(div);
	$(this).addClass('rotate_mill');
    });
}

function sb_cb_init() {
    $('#plus_btn').on('animationend webkitAnimationEnd MSAnimationEnd oAnimationEnd', function(e) {
	console.log(this);
        $(this).removeClass('rotate_mill');
    });
    $('.super_button').on('animationend webkitAnimationEnd MSAnimationEnd oAnimationEnd', function(e) {
	if (flag)
	{
	    $(this).removeClass('fadeOutLeft').css({"display":"none"});
	}
	else
	{
	    $(this).removeClass('fadeInLeft').css({"display":"inherit"});
	}
	animation_cnt--;
	console.log(animation_cnt);
	if (animation_cnt == 0)
	{
	    flag = !flag;
	    console.log(flag);
	}
    });
}

$(document).ready(function(){
    plus_btn_clicked();
    sb_cb_init();
});

