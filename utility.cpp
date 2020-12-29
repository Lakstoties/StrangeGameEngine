//
//  Include the Utility header
//
#include "include\SGE\utility.h"

//
//  Include the System header
//
#include "include\SGE\system.h"

#include <cmath>

//
//  Strange Game Engine Main Namespace
//
namespace SGE::Utility
{
    namespace Math
    {
        //
        //  Precalculated Arrays for Common Math Functions
        //

        float SineDegree[PRECALCULATED_DEGREE_RESOLUTION];

        float CosineDegree[PRECALCULATED_DEGREE_RESOLUTION];

        void InitializeMath()
        {
            //Calculate radian to frequency step for a 1Hz full sine wave
            float periodStepping = TWO_PI_FLOAT / PRECALCULATED_DEGREE_RESOLUTION;

            //
            //  Precalculate the values for the arrays
            //

            for (int i = 0; i < PRECALCULATED_DEGREE_RESOLUTION; i++)
            {
                //
                //Generate Sine Values for Array
                //

                SineDegree[i] = sinf(periodStepping * (float)i);

                //
                //Generate Cosine Value for Array
                //

                CosineDegree[i] = cosf(periodStepping * (float)i);
            }
        }




        void RotatePointAroundPoint(int originalPointX, int originalPointY, int centerPointX, int centerPointY, int &rotatedPointX, int &rotatedPointY, float degrees)
        {
            //Scale degrees to precalculated array range
            int rotationIndex =	(int)(((degrees * PRECALCULATED_DEGREE_RESOLUTION) / 360) + PRECALCULATED_DEGREE_RESOLUTION) % PRECALCULATED_DEGREE_RESOLUTION;


            rotatedPointX = (int)(CosineDegree[rotationIndex] * (float)(originalPointX - centerPointX) - SineDegree[rotationIndex] * (float)(originalPointY - centerPointY) + (float)centerPointX);
            rotatedPointY = (int)(SineDegree[rotationIndex] * (float)(originalPointX - centerPointX) + CosineDegree[rotationIndex] * (float)(originalPointY - centerPointY) + (float)centerPointY);

        }
    }


    //
    //  Namespace for various timing elements
    //
    namespace Timer
    {
        void TimerDelta::Start(float rateOfChange)
        {
            //  Save the rate
            Rate = rateOfChange;

            //  Capture the start time
            StartTime = std::chrono::high_resolution_clock::now();

            //  Flag the timer has started
            started = true;
        }

        float TimerDelta::Stop()
        {
            if (started)
            {
                //  Flag the timer has stopped
                started = false;

                //  Return the delta in
                //
                //  Summary:  Calculated the time delta, cast it down to milliseconds, and get the count of milliseconds, then multiply the rate, and divide by a 1000 milliseconds in a second
                //
                return Rate * std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - StartTime).count() / (float)1000;
            }
            else
            {
                return 0.0f;
            }
        }
    }



    //
    //  Tracker Music Module components
    //
    namespace ModuleTrackerMusic
    {
        //
        //  Module File Player
        //

        float PeriodToOffsetIncrement(unsigned int period)
        {
            return  NTSC_TUNING / (float)(period * SGE::Sound::SAMPLE_RATE * 2);
        }

        unsigned int OffsetIncrementToPeriod(float offsetIncrement)
        {
            return (unsigned int) (NTSC_TUNING / (offsetIncrement * SGE::Sound::SAMPLE_RATE * 2));
        }


        void ModulePlayer::Effect::Parse(unsigned short rawEffectData)
        {
            Type = (rawEffectData & 0x0F00u) >> 8u;
            X = (rawEffectData & 0x00F0u) >> 4u;
            Y = (rawEffectData & 0x000Fu);
        }

        void ModulePlayer::Effect::Reset()
        {
            Type = 0;
            X = 0;
            Y = 0;
        }


        void ModulePlayer::ModuleLocation::SetNextJump(unsigned char position, unsigned char division)
        {
            if (position < NumberOfPositions && division < 64)
            {
                JumpPosition = position;
                JumpDivision = division;
                JumpNext = true;
            }
        }

        void ModulePlayer::ModuleLocation::NextDivision()
        {
            if (!EndOfModule)
            {
                if (JumpNext)
                {
                    Position = JumpPosition;
                    Division = JumpDivision;
                    JumpNext = false;
                }
                else
                {
                    // Increment to the next Division
                    Division++;

                    //  Check to see if we've reached the end
                    if (Division >= 64)
                    {
                        // Next Position, reset the Division
                        Division = 0;

                        if ((Position + 1) >= NumberOfPositions)
                        {
                            EndOfModule = true;
                        }
                        else
                        {
                            // Increment to the next Position
                            Position++;
                        }
                    }
                }
            }
        }

        void ModulePlayer::ModuleLocation::SetDivision(unsigned char division)
        {
            if (division < 64)
            {
                Division = division;
            }
        }

        void ModulePlayer::ModuleLocation::SetPosition(unsigned char position)
        {
            if (position < NumberOfPositions)
            {
                Position = position;
                Division = 0;
            }
        }

        void ModulePlayer::ModuleLocation::Reset()
        {
            Position = 0;
            Division = 0;
            EndOfModule = false;
            JumpNext = false;
        }


        bool ModulePlayer::Load(char * filename)
        {
            //Load it up.
            modFile.LoadFile(filename);

            //If we get here, stuff is okay.
            return modFile.ProperlyLoaded;
        }

        bool ModulePlayer::Connect(unsigned int startChannel, unsigned int startSample)
        {
            //Check to see if we have a valid start position when it comes to channel mapping
            if (startChannel > (SGE::Sound::MAX_CHANNELS - 4))
            {
                return false;
            }

            //Check to see if we have a valid start position when it comes to sample mapping
            if (startSample > (SGE::Sound::MAX_SAMPLE_BUFFERS - 31))
            {
                return false;
            }


            //Map up samples
            for (int i = 0; i < 31; i++)
            {
                sampleMap[i] = i + (int)startSample;
            }

            //Map up channels
            for (int i = 0; i < 4; i++)
            {
                channelMap[i] = i + (int)startChannel;
            }

            //Temporary pointer for converted sample data
            short * temp = nullptr;


            //Convert samples over, load them up
            for (int i = 0; i < 31; i++)
            {
                if (modFile.samples[i].SampleLengthInBytes() > 2)
                {
                    //Allocate memory to the size we need.
                    temp = new SGE::Sound::sampleType[modFile.samples[i].SampleLengthInBytes()];

                    //Convert the sample data
                    modFile.samples[i].ConvertSampleTo16Bit(temp);

                    //Load the convert sample data into the sample buffer
                    SGE::Sound::SampleBuffers[sampleMap[i]].LoadRaw(modFile.samples[i].SampleLengthInBytes(), temp);

                    //Get rid of the old buffer.
                    delete[] temp;
                }

                //Apply repeat data
                SGE::Sound::SampleBuffers[sampleMap[i]].RepeatOffset = (float) modFile.samples[i].repeatOffset * 2;

                //Because of mod file weirdness check to make sure the duration is more than 1.
                SGE::Sound::SampleBuffers[sampleMap[i]].RepeatDuration = (float) (modFile.samples[i].repeatLength > 1 ? modFile.samples[i].repeatLength * 2 : 0);
            }



            //Since this a mod, configure the volume balances for the channels
            SGE::Sound::Channels[channelMap[0]].Pan = 0.50f;
            SGE::Sound::Channels[channelMap[1]].Pan = -0.50f;
            SGE::Sound::Channels[channelMap[2]].Pan = -0.50f;
            SGE::Sound::Channels[channelMap[3]].Pan = 0.50f;

            //If we get here, stuff is okay.
            return true;
        }

        bool ModulePlayer::Play()
        {
            //  Do we even have a file to play?
            if (!modFile.ProperlyLoaded)
            {
                return false;
            }

            //If the player thread is already active... don't pester it
            if (PlayerThreadActive)
            {
                return false;
            }

            //Set the player thread to be active
            PlayerThreadActive = true;

            //Launch the player thread
            if (playerThread == nullptr)
            {
                playerThread = new std::thread(&ModulePlayer::PlayThread, this);
            }

            //If we get here, stuff is okay.
            return true;
        }

        bool ModulePlayer::Stop()
        {
            //
            //  Check to see if the player thread has already been terminated.
            //
            if (playerThread == nullptr)
            {
                //
                //  Thread already taken care of, nothing should be player.
                //
                return true;
            }


            //Set the player thread to be inactive
            PlayerThreadActive = false;

            //Wait for the player thread to join
            if (playerThread->joinable())
            {
                playerThread->join();
            }

            //
            //  Delete the old thread
            //

            delete playerThread;

            playerThread = nullptr;

            //If we get here, stuff is okay.
            return true;
        }

        void ModulePlayer::ProcessDivision()
        {
            //
            //  Check all the channels for any changes
            //
            for (int CurrentChannel = 0; CurrentChannel < 4; CurrentChannel++)
            {

                //
                //  Check for any effect changes on the channel.
                //

                //Parse out the effect
                Channel[CurrentChannel].Effect.Parse(modFile.patterns[modFile.patternTable[Location.Position]][Location.Division][CurrentChannel].effect);

                //
                //  Check for period changes on each channel
                //


                //
                //  Check to see if period not zero
                //  If it is zero don't change the period used in this channel
                //  NOTE:  Effect 3 does some weird stuff, the period is target note to shift to but it does NOT trigger a play effect

                if (modFile.patterns[modFile.patternTable[Location.Position]][Location.Division][CurrentChannel].period != 0)
                {
                    //Set the period
                    Channel[CurrentChannel].Period = modFile.patterns[modFile.patternTable[Location.Position]][Location.Division][CurrentChannel].period;

                    //
                    //   Period Slides don't actually play the note, but use the period as a value for the effect
                    //
                    if (Channel[CurrentChannel].Effect.Type != 0x3 && Channel[CurrentChannel].Effect.Type != 0x5)
                    {
                        //Convert the period to offset timing interval in relation to system sampling rate
                        //Using NTSC sampling
                        SGE::Sound::Channels[channelMap[CurrentChannel]].offsetIncrement = PeriodToOffsetIncrement(Channel[CurrentChannel].Period);
                    }

                    // If there's a period listed, we play the sample new.
                    SGE::Sound::Channels[channelMap[CurrentChannel]].Play();
                }

                //
                //  Check for sample changes on each channel
                //
                if (modFile.patterns[modFile.patternTable[Location.Position]][Location.Division][CurrentChannel].sample != 0)
                {
                    //
                    //  Check to see if the previous sample was the same as the new one
                    //

                    if (Channel[CurrentChannel].Sample != modFile.patterns[modFile.patternTable[Location.Position]][Location.Division][CurrentChannel].sample)
                    {
                        //Otherwise, channel up the sample used, effectively resetting the channel to the sample settings.
                        //Stop this channel
                        SGE::Sound::Channels[channelMap[CurrentChannel]].Stop();

                        // Set the sample
                        Channel[CurrentChannel].Sample = modFile.patterns[modFile.patternTable[Location.Position]][Location.Division][CurrentChannel].sample;

                        //Switch to the sample
                        SGE::Sound::Channels[channelMap[CurrentChannel]].SetSampleBuffer(sampleMap[Channel[CurrentChannel].Sample - 1]);

                        //Start playing the new sample
                        SGE::Sound::Channels[channelMap[CurrentChannel]].Play();
                    }

                    //Set sample volume
                    SGE::Sound::Channels[channelMap[CurrentChannel]].Volume = float(modFile.samples[Channel[CurrentChannel].Sample - 1].volume) / 64.0f;
                }



                //
                //Turn off any channel based rendering effects for now
                //If there's suppose to be an effect, it should be configured in the next heap of logic.
                //

                //Turn off Arpeggio
                SGE::Sound::Channels[channelMap[CurrentChannel]].Arpeggio.Enabled = false;

                //Turn off Volume Slide
                SGE::Sound::Channels[channelMap[CurrentChannel]].VolumeSlide.Enabled = false;

                //Turn off Vibrato
                SGE::Sound::Channels[channelMap[CurrentChannel]].Vibrato.Enabled = false;

                //Turn off Period Slide
                SGE::Sound::Channels[channelMap[CurrentChannel]].PeriodSlide.Enabled = false;

                //Turn off Retrigger Sample
                SGE::Sound::Channels[channelMap[CurrentChannel]].Retrigger.Enabled = false;

                //
                //Is there an effect at all?
                //If the whole effect value is 0, then there is no effect
                //
                if (modFile.patterns[modFile.patternTable[Location.Position]][Location.Division][CurrentChannel].effect != 0)
                {
                    //Since the effect value is non-zero, there's some kind of effect data to be processed!
                    //Let's find it!

                    //  Configure Arpeggio or Effect 0 / 0x0
                    if (Channel[CurrentChannel].Effect.Type == 0x0)
                    {

                        SGE::Sound::Channels[channelMap[CurrentChannel]].Arpeggio.Set(
                                DEFAULT_SAMPLES_TICK,
                                Channel[CurrentChannel].Effect.X,
                                Channel[CurrentChannel].Effect.Y);
                    }
                        //  Configure Slide Up or Effect 1 / 0x1
                    else if (Channel[CurrentChannel].Effect.Type == 0x1)
                    {
                        //  NOTE:  Slides are divided by 42 to allow smoother transitions.  It is the answer.
                        SGE::Sound::Channels[channelMap[CurrentChannel]].PeriodSlide.Set(
                                (float)MOD_NOTE_PERIODS::B3,
                                DEFAULT_SAMPLES_TICK / 42,
                                (float)(Channel[CurrentChannel].Effect.X * 16 + Channel[CurrentChannel].Effect.Y) / 42.0f,
                                NTSC_TUNING / 2);
                    }
                        //  Configure Slide Down or Effect 2 / 0x2
                    else if (Channel[CurrentChannel].Effect.Type == 0x2)
                    {
                        //  NOTE:  Slides are divided by 42 to allow smoother transitions.  It is the answer.
                        SGE::Sound::Channels[channelMap[CurrentChannel]].PeriodSlide.Set(
                                (float)MOD_NOTE_PERIODS::C1,
                                DEFAULT_SAMPLES_TICK / 42,
                                (float)(Channel[CurrentChannel].Effect.X * 16 + Channel[CurrentChannel].Effect.Y) / 42.0f,
                                NTSC_TUNING / 2);
                    }
                        //  Configure the Slide to Note of Effect 3 / 0x3
                    else if (Channel[CurrentChannel].Effect.Type == 0x3)
                    {
                        //  NOTE:  Slides are divided by 42 to allow smoother transitions.  It is the answer.

                        //  See if there's a new slide
                        if (Channel[CurrentChannel].Effect.X != 0 || Channel[CurrentChannel].Effect.Y != 0)
                        {
                            SGE::Sound::Channels[channelMap[CurrentChannel]].PeriodSlide.Set(
                                    (float)Channel[CurrentChannel].Period,
                                    DEFAULT_SAMPLES_TICK / 42,
                                    (float)(Channel[CurrentChannel].Effect.X * 16 + Channel[CurrentChannel].Effect.Y) / 42.0f,
                                    NTSC_TUNING / 2);
                        }

                            // Or continue the previous one towards the current note
                        else
                        {
                            SGE::Sound::Channels[channelMap[CurrentChannel]].PeriodSlide.Set(
                                    (float)Channel[CurrentChannel].Period,
                                    SGE::Sound::Channels[channelMap[CurrentChannel]].PeriodSlide.SampleInterval,
                                    SGE::Sound::Channels[channelMap[CurrentChannel]].PeriodSlide.Delta,
                                    NTSC_TUNING / 2);
                        }
                    }
                        //  Configure the Vibrato Effect or Effect 4 / 0x4
                    else if (Channel[CurrentChannel].Effect.Type == 0x4)
                    {
                        //  If either the Effect X or Y is Zero, then use the previous Vibrato settings
                        if (Channel[CurrentChannel].Effect.X == 0 || Channel[CurrentChannel].Effect.Y == 0)
                        {
                            float tempOffsetIncrement = PeriodToOffsetIncrement(Channel[CurrentChannel].Period);

                            SGE::Sound::Channels[channelMap[CurrentChannel]].Vibrato.Set(
                                    tempOffsetIncrement * std::pow(SGE::Sound::Precalculated::SEMITONE_MULTIPLIER, (float)previousEffect[CurrentChannel].Y / 16.0f) - tempOffsetIncrement,
                                    ((float)(previousEffect[CurrentChannel].X * ticksADivision) / 64.0f) * (1000.0f / (float)(DEFAULT_TICK_TIMING_MILLI * ticksADivision)));
                        }

                            //  Otherwise it is new Vibrato!
                        else
                        {
                            float tempOffsetIncrement = PeriodToOffsetIncrement(Channel[CurrentChannel].Period);

                            SGE::Sound::Channels[channelMap[CurrentChannel]].Vibrato.Set(
                                    tempOffsetIncrement * std::pow(SGE::Sound::Precalculated::SEMITONE_MULTIPLIER, (float)Channel[CurrentChannel].Effect.Y / 16.0f) - tempOffsetIncrement,
                                    ((float)(Channel[CurrentChannel].Effect.X * ticksADivision) / 64.0f) * (1000.0f / (float)(DEFAULT_TICK_TIMING_MILLI * ticksADivision)));

                            //  Save the Vibrato settings to use in the future
                            previousEffect[CurrentChannel] = Channel[CurrentChannel].Effect;
                        }
                    }
                        //  Configure - Continue Slide to note, but do Volume Slide
                    else if (Channel[CurrentChannel].Effect.Type == 0x5)
                    {

                        SGE::Sound::Channels[channelMap[CurrentChannel]].PeriodSlide.Set(
                                (float)Channel[CurrentChannel].Period,
                                SGE::Sound::Channels[channelMap[CurrentChannel]].PeriodSlide.SampleInterval,
                                SGE::Sound::Channels[channelMap[CurrentChannel]].PeriodSlide.Delta,
                                NTSC_TUNING);

                        //  Check to see the rate we have to slide the volume up
                        if (Channel[CurrentChannel].Effect.X != 0)
                        {
                            SGE::Sound::Channels[channelMap[CurrentChannel]].VolumeSlide.Set(
                                    DEFAULT_SAMPLES_TICK,
                                    (float)Channel[CurrentChannel].Effect.X / 64.0f);
                        }
                            //  Check to see the rate we have to slide the volume down
                            //  Y is only paid attention if X is zero and is there for assumed to be zero
                        else if (Channel[CurrentChannel].Effect.Y != 0)
                        {
                            SGE::Sound::Channels[channelMap[CurrentChannel]].VolumeSlide.Set(
                                    DEFAULT_SAMPLES_TICK,
                                    (float)-Channel[CurrentChannel].Effect.Y / 64.0f);
                        }
                    }
                        //  Configure - Continue Vibrato, but do Volume Slide
                    else if (Channel[CurrentChannel].Effect.Type == 0x6)
                    {
                        //  Check to see the rate we have to slide the volume up
                        if (Channel[CurrentChannel].Effect.X != 0)
                        {
                            SGE::Sound::Channels[channelMap[CurrentChannel]].VolumeSlide.Set(
                                    DEFAULT_SAMPLES_TICK,
                                    (float)Channel[CurrentChannel].Effect.X / 64.0f);
                        }

                            //  Check to see the rate we have to slide the volume down
                            //  Y is only paid attention if X is zero and is there for assumed to be zero
                        else if (Channel[CurrentChannel].Effect.Y != 0)
                        {
                            SGE::Sound::Channels[channelMap[CurrentChannel]].VolumeSlide.Set(
                                    DEFAULT_SAMPLES_TICK,
                                    (float)-Channel[CurrentChannel].Effect.Y / 64.0f);
                        }
                    }
                    else if (Channel[CurrentChannel].Effect.Type == 0x7)
                    {
                        //
                        //  Place holder for Tremolo
                        //
                        SGE::System::Message::Output(SGE::System::Message::Levels::Warning, SGE::System::Message::Sources::Utility, "Mod Player - Unimplemented or Unknown effect detected! Effect: " + std::to_string(Channel[CurrentChannel].Effect.Type) + " X: " + std::to_string(Channel[CurrentChannel].Effect.X) + " Y: " + std::to_string(Channel[CurrentChannel].Effect.Y) + "\n");
                    }
                    else if (Channel[CurrentChannel].Effect.Type == 0x8)
                    {
                        //
                        //  Place holder for Pan
                        //  Rarely implemented
                        //
                        SGE::System::Message::Output(SGE::System::Message::Levels::Warning, SGE::System::Message::Sources::Utility, "Mod Player - Unimplemented or Unknown effect detected! Effect: " + std::to_string(Channel[CurrentChannel].Effect.Type) + " X: " + std::to_string(Channel[CurrentChannel].Effect.X) + " Y: " + std::to_string(Channel[CurrentChannel].Effect.Y) + "\n");
                    }
                        //  Set sample offset
                    else if (Channel[CurrentChannel].Effect.Type == 0x9)
                    {

                        SGE::Sound::Channels[channelMap[CurrentChannel]].offset = (float)(Channel[CurrentChannel].Effect.X * 4096 + Channel[CurrentChannel].Effect.Y * 256) * 2;
                    }
                        //  Volume Slide Effect
                    else if (Channel[CurrentChannel].Effect.Type == 0xA)
                    {
                        //  Check to see the rate we have to slide the volume up
                        if (Channel[CurrentChannel].Effect.X != 0)
                        {
                            SGE::Sound::Channels[channelMap[CurrentChannel]].VolumeSlide.Set(
                                    DEFAULT_SAMPLES_TICK,
                                    (float)Channel[CurrentChannel].Effect.X / 64.0f);
                        }

                            //  Check to see the rate we have to slide the volume down
                            //  Y is only paid attention if X is zero and is there for assumed to be zero
                        else if (Channel[CurrentChannel].Effect.Y != 0)
                        {
                            SGE::Sound::Channels[channelMap[CurrentChannel]].VolumeSlide.Set(
                                    DEFAULT_SAMPLES_TICK,
                                    (float)-Channel[CurrentChannel].Effect.Y / 64.0f);
                        }
                    }
                        //  Configure for Jump position after this division or Effect 11 / 0xB
                    else if (Channel[CurrentChannel].Effect.Type == 0xB)
                    {
                        Location.SetNextJump((Channel[CurrentChannel].Effect.X * 16 + Channel[CurrentChannel].Effect.Y), 0);
                    }
                        //  Configure the Volume or Effect 12 / 0xC
                    else if (Channel[CurrentChannel].Effect.Type == 0xC)
                    {
                        //  Set the volume for the channel
                        SGE::Sound::Channels[channelMap[CurrentChannel]].Volume = (float)(Channel[CurrentChannel].Effect.X * 16 + Channel[CurrentChannel].Effect.Y) / 64.0f;

                        //  If for some reason the channel volume is higher than 1.0f, set it back down.
                        if (SGE::Sound::Channels[channelMap[CurrentChannel]].Volume > 1.0f)
                        {
                            SGE::Sound::Channels[channelMap[CurrentChannel]].Volume = 1.0f;
                        }
                    }
                        //  Configure pattern break or Effect 13 / 0xD
                    else if (Channel[CurrentChannel].Effect.Type == 0xD)
                    {
                        Location.SetNextJump(Location.Position + 1, (Channel[CurrentChannel].Effect.X * 10 + Channel[CurrentChannel].Effect.Y));
                    }

                        //  Configure an effect under this category.  There's a few of them shove under Effect 14 or 0xE
                    else if (Channel[CurrentChannel].Effect.Type == 0xE)
                    {
                        //
                        //  Check the Effect's X setting to see what exact effect they want
                        //


                        //  This is for enabling or disabling a lowpass filter on actual hardware
                        //  Most people usually just disabled this thing and moved on.
                        if (Channel[CurrentChannel].Effect.X == 0x0)
                        {
                            //
                            //  Do nothing...  NOOP!
                            //
                        }
                        else if (Channel[CurrentChannel].Effect.X == 0x1)
                        {
                            //  Place holder for Fineslide up
                            SGE::System::Message::Output(SGE::System::Message::Levels::Warning, SGE::System::Message::Sources::Utility, "Mod Player - Unimplemented or Unknown effect detected! Effect: " + std::to_string(Channel[CurrentChannel].Effect.Type) + " X: " + std::to_string(Channel[CurrentChannel].Effect.X) + " Y: " + std::to_string(Channel[CurrentChannel].Effect.Y) + "\n");
                        }
                        else if (Channel[CurrentChannel].Effect.X == 0x2)
                        {
                            //  Place holder for Fineslide down
                            SGE::System::Message::Output(SGE::System::Message::Levels::Warning, SGE::System::Message::Sources::Utility, "Mod Player - Unimplemented or Unknown effect detected! Effect: " + std::to_string(Channel[CurrentChannel].Effect.Type) + " X: " + std::to_string(Channel[CurrentChannel].Effect.X) + " Y: " + std::to_string(Channel[CurrentChannel].Effect.Y) + "\n");
                        }
                        else if (Channel[CurrentChannel].Effect.X == 0x3)
                        {
                            //  Place holder for Glissasndo On/Off
                            SGE::System::Message::Output(SGE::System::Message::Levels::Warning, SGE::System::Message::Sources::Utility, "Mod Player - Unimplemented or Unknown effect detected! Effect: " + std::to_string(Channel[CurrentChannel].Effect.Type) + " X: " + std::to_string(Channel[CurrentChannel].Effect.X) + " Y: " + std::to_string(Channel[CurrentChannel].Effect.Y) + "\n");
                        }
                            //  Set the Vibrato waveform
                        else if (Channel[CurrentChannel].Effect.X == 0x4)
                        {
                            // Set the waveform to use
                            if (Channel[CurrentChannel].Effect.Y == 0 || Channel[CurrentChannel].Effect.Y == 4)
                            {
                                // Use Sine waveform with retrigger
                                SGE::Sound::Channels[channelMap[CurrentChannel]].Vibrato.Waveform(SGE::Sound::Channel::VibratoEffect::VibratoWaveforms::Sine);
                            }
                            else if (Channel[CurrentChannel].Effect.Y == 1 || Channel[CurrentChannel].Effect.Y == 5)
                            {
                                // Use Ramp Down waveform with retrigger
                                SGE::Sound::Channels[channelMap[CurrentChannel]].Vibrato.Waveform(SGE::Sound::Channel::VibratoEffect::VibratoWaveforms::RampDown);
                            }
                            else if (Channel[CurrentChannel].Effect.Y == 2 || Channel[CurrentChannel].Effect.Y == 6)
                            {
                                // Use Square waveform with trigger
                                SGE::Sound::Channels[channelMap[CurrentChannel]].Vibrato.Waveform(SGE::Sound::Channel::VibratoEffect::VibratoWaveforms::Square);
                            }
                            else if (Channel[CurrentChannel].Effect.Y == 3)
                            {
                                //  Place holder for random selection with retrigger
                                SGE::System::Message::Output(SGE::System::Message::Levels::Warning, SGE::System::Message::Sources::Utility, "Mod Player - Unimplemented or Unknown effect detected! Effect: " + std::to_string(Channel[CurrentChannel].Effect.Type) + " X: " + std::to_string(Channel[CurrentChannel].Effect.X) + " Y: " + std::to_string(Channel[CurrentChannel].Effect.Y) + "\n");
                            }

                            // Set if it triggers or not
                            if (Channel[CurrentChannel].Effect.Y >= 4)
                            {
                                SGE::Sound::Channels[channelMap[CurrentChannel]].Vibrato.Retriggers = false;
                            }
                            else
                            {
                                SGE::Sound::Channels[channelMap[CurrentChannel]].Vibrato.Retriggers = true;
                            }
                        }
                        else if (Channel[CurrentChannel].Effect.X == 0x5)
                        {
                            //  Place holder for Set finetune value
                            SGE::System::Message::Output(SGE::System::Message::Levels::Warning, SGE::System::Message::Sources::Utility, "Mod Player - Unimplemented or Unknown effect detected! Effect: " + std::to_string(Channel[CurrentChannel].Effect.Type) + " X: " + std::to_string(Channel[CurrentChannel].Effect.X) + " Y: " + std::to_string(Channel[CurrentChannel].Effect.Y) + "\n");
                        }
                        else if (Channel[CurrentChannel].Effect.X == 0x6)
                        {
                            //  Place holder for Loop Pattern
                            SGE::System::Message::Output(SGE::System::Message::Levels::Warning, SGE::System::Message::Sources::Utility, "Mod Player - Unimplemented or Unknown effect detected! Effect: " + std::to_string(Channel[CurrentChannel].Effect.Type) + " X: " + std::to_string(Channel[CurrentChannel].Effect.X) + " Y: " + std::to_string(Channel[CurrentChannel].Effect.Y) + "\n");
                        }
                        else if (Channel[CurrentChannel].Effect.X == 0x7)
                        {
                            //  Place holder for Set Tremolo Waveform
                            SGE::System::Message::Output(SGE::System::Message::Levels::Warning, SGE::System::Message::Sources::Utility, "Mod Player - Unimplemented or Unknown effect detected! Effect: " + std::to_string(Channel[CurrentChannel].Effect.Type) + " X: " + std::to_string(Channel[CurrentChannel].Effect.X) + " Y: " + std::to_string(Channel[CurrentChannel].Effect.Y) + "\n");
                        }
                        else if (Channel[CurrentChannel].Effect.X == 0x8)
                        {
                            //  Unused
                            SGE::System::Message::Output(SGE::System::Message::Levels::Warning, SGE::System::Message::Sources::Utility, "Mod Player - Unimplemented or Unknown effect detected! Effect: " + std::to_string(Channel[CurrentChannel].Effect.Type) + " X: " + std::to_string(Channel[CurrentChannel].Effect.X) + " Y: " + std::to_string(Channel[CurrentChannel].Effect.Y) + "\n");
                        }
                            //  Retrigger Sample
                        else if (Channel[CurrentChannel].Effect.X == 0x9)
                        {

                            if (Channel[CurrentChannel].Effect.Y != 0)
                            {
                                SGE::Sound::Channels[channelMap[CurrentChannel]].Retrigger.Set(0, Channel[CurrentChannel].Effect.Y* DEFAULT_SAMPLES_TICK);
                            }
                        }
                            //  Fine volume slide up
                        else if (Channel[CurrentChannel].Effect.X == 0xA)
                        {
                            //  Increment current volume by y
                            SGE::Sound::Channels[channelMap[CurrentChannel]].Volume += (float)(Channel[CurrentChannel].Effect.Y) / 64.0f;

                            //  Check to make sure the volume doesn't go over 1.0
                            if (SGE::Sound::Channels[channelMap[CurrentChannel]].Volume > 1.0f)
                            {
                                SGE::Sound::Channels[channelMap[CurrentChannel]].Volume = 1.0f;
                            }
                        }
                            //  Fine volume slide down
                        else if (Channel[CurrentChannel].Effect.X == 0xB)
                        {
                            //  Increment current volume by y
                            SGE::Sound::Channels[channelMap[CurrentChannel]].Volume -= (float)(Channel[CurrentChannel].Effect.Y) / 64.0f;

                            //  Check to make sure the volume doesn't go over 1.0
                            if (SGE::Sound::Channels[channelMap[CurrentChannel]].Volume < 0.0f)
                            {
                                SGE::Sound::Channels[channelMap[CurrentChannel]].Volume = 0.0f;
                            }
                        }
                            //  Cut sample
                        else if (Channel[CurrentChannel].Effect.X == 0xC)
                        {

                            SGE::Sound::Channels[channelMap[CurrentChannel]].Cut.Set(Channel[CurrentChannel].Effect.Y * DEFAULT_SAMPLES_TICK);
                        }
                            //  Delay sample
                        else if (Channel[CurrentChannel].Effect.X == 0xD)
                        {

                            SGE::Sound::Channels[channelMap[CurrentChannel]].Delay.Set(Channel[CurrentChannel].Effect.Y * DEFAULT_SAMPLES_TICK);
                        }
                        else if (Channel[CurrentChannel].Effect.X == 0xE)
                        {
                            //  Place holder for delay pattern
                            SGE::System::Message::Output(SGE::System::Message::Levels::Warning, SGE::System::Message::Sources::Utility, "Mod Player - Unimplemented or Unknown effect detected! Effect: " + std::to_string(Channel[CurrentChannel].Effect.Type) + " X: " + std::to_string(Channel[CurrentChannel].Effect.X) + " Y: " + std::to_string(Channel[CurrentChannel].Effect.Y) + "\n");
                        }
                        else if (Channel[CurrentChannel].Effect.X == 0xF)
                        {
                            //  Place holder for invert loop
                            SGE::System::Message::Output(SGE::System::Message::Levels::Warning, SGE::System::Message::Sources::Utility, "Mod Player - Unimplemented or Unknown effect detected! Effect: " + std::to_string(Channel[CurrentChannel].Effect.Type) + " X: " + std::to_string(Channel[CurrentChannel].Effect.X) + " Y: " + std::to_string(Channel[CurrentChannel].Effect.Y) + "\n");
                        }
                    }

                        //  Configure Ticks Per Division or Effect 15 / 0xF
                    else if (Channel[CurrentChannel].Effect.Type == 0xF)
                    {
                        //  If the value is 32 or below, consider it Ticks Per Division setting
                        if (Channel[CurrentChannel].Effect.X * 16 + Channel[CurrentChannel].Effect.Y <= 32)
                        {
                            ticksADivision = Channel[CurrentChannel].Effect.X * 16 + Channel[CurrentChannel].Effect.Y;
                        }
                            //  If the value is above 32, consider it setting the beats per minute
                        else
                        {

                        }
                    }
                }
            }
        }

        void ModulePlayer::PlayThread()
        {
            //  Default for most mods, can be changed.
            ticksADivision = DEFAULT_TICKS_A_DIVISION;

            //  Statistics information for tick rate
            std::chrono::high_resolution_clock::time_point startTime;
            std::chrono::high_resolution_clock::duration deltaTime;
            std::chrono::high_resolution_clock::time_point targetTime;

            std::chrono::high_resolution_clock::time_point startSleepTime;
            std::chrono::high_resolution_clock::duration deltaSleepTime = std::chrono::milliseconds (0);

            //  Previous Effect Settings
            previousEffect[0].Reset();
            previousEffect[1].Reset();
            previousEffect[2].Reset();
            previousEffect[3].Reset();

            //  Start playback processing
            while (PlayerThreadActive)
            {
                SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::Utility, "Mod Player: Starting to play: " + std::string(modFile.title) + "\n");
                SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::Utility, "Mod Player: Song Positions: " + std::to_string(modFile.songPositions) + "\n");

                //  Load up the Location data
                Location.Reset();
                Location.NumberOfPositions = modFile.songPositions;

                //  While we are not at the end of the song.
                while (!Location.EndOfModule && PlayerThreadActive)
                {
                    //Save the timer to help time the processing time for this division
                    startTime = std::chrono::high_resolution_clock::now();

                    //
                    //  Set up the channels
                    //
                    SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::Utility, "Mod Player: " + std::string(modFile.title) + " - Pat: " + std::to_string(Location.Position) + " - Div: " + std::to_string(Location.Division) + " - Time: " + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(deltaTime).count()) + "\n");

                    //
                    //  Process this division
                    //

                    this->ProcessDivision();

                    //
                    //  Wait for the next division
                    //

                    //  Calculate when the next division should happen based on ticksPerDivision and latest startTime
                    targetTime = startTime + std::chrono::milliseconds (DEFAULT_TICK_TIMING_MILLI * ticksADivision);

                    //
                    //  Attempt a sleep, since we're assuming that DEFAULT_TICK_TIMING_MILLI is larger the OS thread scheduler time slice
                    //  Record delta sleep time to guess if we have enough time to sleep again.
                    while ((std::chrono::high_resolution_clock::now() + deltaSleepTime) < targetTime)
                    {
                        startSleepTime = std::chrono::high_resolution_clock::now();
                        std::this_thread::sleep_for(std::chrono::milliseconds(1));
                        deltaSleepTime = std::chrono::high_resolution_clock::now() - startSleepTime;
                    }

                    //  Spin locker option
                    //  Super accurate...  Lights CPU Cycles... ON FIRE!
                    while (std::chrono::high_resolution_clock::now() < targetTime)
                    {
                        std::this_thread::yield();
                    }


                    //SGE::System::OS::AccurateWaitForMilliseconds(DEFAULT_TICK_TIMING_MILLI * ticksADivision);


                    //
                    //  Calculate the delta time, post sleep
                    //
                    deltaTime = std::chrono::high_resolution_clock::now() - startTime;


                    //
                    //  Go to the next division
                    //
                    Location.NextDivision();
                }
            }

            //
            //  Post Playback processing
            //

            //Stop all channels
            for (int c : channelMap)
            {
                SGE::Sound::Channels[c].Stop();
            }
        }


        //Constructor Stuff
        ModulePlayer::ModulePlayer() = default;

        ModulePlayer::~ModulePlayer()
        {
            //Stop the player
            this->Stop();
        }
    }
}
