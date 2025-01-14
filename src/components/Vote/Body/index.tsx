/* eslint-disable @typescript-eslint/no-explicit-any */
import { BlockProducerItem } from '@edenia/ui-kit'
import { Link, Typography } from '@mui/material'
import { useTranslation } from 'next-i18next'
import { useState } from 'react'

import { socialMediaInfo } from 'config/constants'
import { formatters } from 'utils'

import useStyles from './styles'

type voteBodyProps = {
  setBps(delegate: any): void
  bps: { sort: string; data: Array<any> }
}

const Body: React.FC<voteBodyProps> = ({ setBps, bps }) => {
  const classes = useStyles()
  const { t } = useTranslation()
  const [seletedAllBps, setSelectedAllBps] = useState<boolean>(true)

  const handleSelected = pressBp => {
    const updatedBpState = bps?.data?.map(bp => {
      if (bp.producer === pressBp) return { ...bp, selected: !bp?.selected }

      return bp
    })

    setBps({ ...bps, data: updatedBpState })
  }

  const selectedAll = () => {
    setBps({
      ...bps,
      data: bps?.data?.map(bp => {
        return { ...bp, selected: seletedAllBps ? false : true }
      })
    })
    setSelectedAllBps(!seletedAllBps)
  }

  return (
    <div className={classes.container}>
      {bps?.data?.length > 0 && (
        <div className={classes.paddingSelectedAll}>
          <label>
            <input
              checked={seletedAllBps}
              className={classes.delegateBpItemCheckbox}
              type='checkbox'
              id='checkbox'
              name='checkbox'
              onChange={() => selectedAll()}
            />
            {t('vote.selectAll')}
          </label>
        </div>
      )}
      {bps?.data?.map(bp => (
        <div key={bp?.producer} className={classes.bpLogoStyle}>
          <BlockProducerItem
            onClick={() => handleSelected(bp?.producer)}
            isSelected={bp?.selected}
            eosrateValue={bp?.eosrateStats?.average.toFixed(1)}
            positionText={
              bp?.totalVotes
                ? `${formatters.formatWithThousandSeparator(
                    bp?.totalVotes,
                    3
                  )} votes`
                : undefined
            }
            avatarIcon={bp?.voted && '/icons/good-icon.png'}
            name={bp?.producer}
            nameFontWeight={600}
            rankValue={bp?.rank}
            image={
              bp?.bpJsonData?.org?.branding?.logo_256 || '/logos/no-logo.svg'
            }
            bgColor='#fff'
            proxyScore={String(bp.stats)}
            selectableItems={
              <div className={classes.socialItems}>
                {bp?.bpJsonData &&
                  Object.entries(bp?.bpJsonData?.org?.social).map(
                    (item, index) => (
                      <Typography key={index} variant='subtitle2'>
                        {index !== 0 && '·'}
                        <Link
                          className={classes.linkPadding}
                          rel='noreferrer'
                          href={`${socialMediaInfo?.links[item[0]]}${item[1]}`}
                          target='_blank'
                        >
                          {item[0]}
                        </Link>
                      </Typography>
                    )
                  )}
              </div>
            }
          />
        </div>
      ))}
    </div>
  )
}

export default Body
