/* AR: init, sense, featurize, classify, log */

PC_Channel< float * > *reading_sense_featurize;
init () {
  // run once, ever
  // allocate space for “globals”
  // runtime internal stuff goes here
  
  /*Arg to ctr is size of template arg -- 
    accomodates arrays -- default: sizeof T*/
  reading_sense_featurize = new PC_Channel<float *>( 3*sizeof(float) );
}





task sense
 IN:(nil) OUT:(reading[3]->featurize, tstamp->featurize)
{
  float x = read_adc();
  float y = read_adc();
  float z = read_adc();
  
  float reading[3] = {x, y, z};

  /*
    The channel is of type Channel< float * >
    and should have space for 3*sizeof(float) 
    to be memcpy'd

    The task tracks a pointer to this entry in the channel so 
    if we fail, we can go back and reconsider these 
    entries again.  when we commit, the tentativity is 
    marked non-tentative.
  */
  reading_sense_featurize->output(reading, 3*sizeof(float) );

  /*
    Things pushed into the channel during this execution of this
    task are no longer tentative after the commit is set.  This
    happens in two phases --
    1)Each entry added by this task is marked non-tentative
    2)The channel itself is marked non-tentative, committing the task's
      state and making further channel writes by the task a fail-stop error
  */
  reading_sense_featurize->commit();

  /*same deal as above, but for the tstamp channel*/
  time_t tstamp;
  tstamp = time(NULL); 
  CHANNEL_out_tstamp_sense_featurize->add( tstamp, sizeof(tstamp) );
  CHANNEL_out_tstamp_sense_featurize->commit();

  // note the “return” should commit any uncommitted outs (e.g. if tstamp wasn't)

  /* runtime will set next state == featurize */
  return featurize; 
}

task featurize
 IN:(self->window, sense->reading[3], sense->tstamp)
 OUT:(mean->classify, variance->classify, window->self)
{
  tmpwindow = window + reading;
  mean = foo(tmpwindow);
  variance = mean / tstamp;
  if (variance < 0) {
    return error; // go to “error” state
  }
  return classify;
}

task classify
 IN:(self->model, featurize->mean, featurize->variance)
 OUT:(model->self, class->log)
{
  clazz = getclass(model, mean, variance); // getclass() is normal fn
}

task log
 IN:(self->log, classify->class)
 OUT:(log->self)
{
}

task error IN:(nil) OUT:(nil) {
}
