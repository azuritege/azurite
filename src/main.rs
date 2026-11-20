mod azurite;
use azurite::context::Context;

use std::time::Instant;

fn main() {
    let mut ctx = Context::new(1280, 720);

    let mut last_frame = Instant::now();
    while !ctx.should_close().get() {
        let now = Instant::now();
        let delta = now.duration_since(last_frame).as_secs_f32();
        last_frame = now;

        ctx.update(delta);
        ctx.render();
    }
}
