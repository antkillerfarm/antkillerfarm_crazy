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
	    $(divs[i]).css({"animation-delay":delay, "-webkit-animation-delay":delay}).addClass('fadeOutLeft');
	}
	else
	{
	    $(divs[i]).css({"animation-delay":delay, "-webkit-animation-delay":delay}).addClass('fadeInLeft');
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
	    $(this).removeClass('fadeOutLeft').css({"opacity":"0", "animation-delay":""});
	}
	else
	{
	    $(this).removeClass('fadeInLeft').css({"opacity":"1", "animation-delay":""});
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

