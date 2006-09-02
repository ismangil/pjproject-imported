/* $Id$ */
/* 
 * Copyright (C) 2003-2006 Benny Prijono <benny@prijono.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 */
#include <pjmedia.h>
#include <pjlib.h>

#define SAMPLES_PER_FRAME   64
#define ON_DURATION	    100
#define OFF_DURATION	    100


/*
 * main()
 */
int main()
{
    pj_caching_pool cp;
    pjmedia_endpt *med_endpt;
    pj_pool_t *pool;
    pjmedia_port *port;
    unsigned i;
    pj_status_t status;


    /* Must init PJLIB first: */
    status = pj_init();
    PJ_ASSERT_RETURN(status == PJ_SUCCESS, 1);

    /* Must create a pool factory before we can allocate any memory. */
    pj_caching_pool_init(&cp, &pj_pool_factory_default_policy, 0);

    /* 
     * Initialize media endpoint.
     * This will implicitly initialize PJMEDIA too.
     */
    status = pjmedia_endpt_create(&cp.factory, NULL, 1, &med_endpt);
    PJ_ASSERT_RETURN(status == PJ_SUCCESS, 1);

    /* Create memory pool for our file player */
    pool = pj_pool_create( &cp.factory,	    /* pool factory	    */
			   "app",	    /* pool name.	    */
			   4000,	    /* init size	    */
			   4000,	    /* increment size	    */
			   NULL		    /* callback on error    */
			   );

    status = pjmedia_tonegen_create(pool, 8000, 1, SAMPLES_PER_FRAME, 16, 0, &port);
    if (status != PJ_SUCCESS)
	return 1;

    {
	pjmedia_tone_desc tones[3];

	tones[0].freq1 = 200;
	tones[0].freq2 = 0;
	tones[0].on_msec = ON_DURATION;
	tones[0].off_msec = OFF_DURATION;

	tones[1].freq1 = 400;
	tones[1].freq2 = 0;
	tones[1].on_msec = ON_DURATION;
	tones[1].off_msec = OFF_DURATION;

	tones[2].freq1 = 800;
	tones[2].freq2 = 0;
	tones[2].on_msec = ON_DURATION;
	tones[2].off_msec = OFF_DURATION;

	status = pjmedia_tonegen_play(port, 3, tones, 0);
	PJ_ASSERT_RETURN(status==PJ_SUCCESS, 1);
    }

    {
	pjmedia_tone_digit digits[2];

	digits[0].digit = '0';
	digits[0].on_msec = ON_DURATION;
	digits[0].off_msec = OFF_DURATION;

	digits[1].digit = '0';
	digits[1].on_msec = ON_DURATION;
	digits[1].off_msec = OFF_DURATION;

	status = pjmedia_tonegen_play_digits(port, 2, digits, 0);
	PJ_ASSERT_RETURN(status==PJ_SUCCESS, 1);
    }

    {
	pjmedia_frame frm;
	FILE *f;
	void *buf;

	buf = pj_pool_alloc(pool, 2*8000);
	frm.buf = buf;

	f = fopen("tonegen.pcm", "wb");

	for (i=0; i<8000/SAMPLES_PER_FRAME; ++i) {
	    pjmedia_port_get_frame(port, &frm);
	    fwrite(buf, SAMPLES_PER_FRAME, 2, f);
	}

	pj_assert(pjmedia_tonegen_is_busy(port) == 0);
	fclose(f);
    }

    /* Delete port */
    pjmedia_port_destroy(port);

    /* Release application pool */
    pj_pool_release( pool );

    /* Destroy media endpoint. */
    pjmedia_endpt_destroy( med_endpt );

    /* Destroy pool factory */
    pj_caching_pool_destroy( &cp );


    /* Done. */
    return 0;
}
